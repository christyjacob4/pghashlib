# Getting Started with pghashlib

This guide will help you get started with pghashlib and show you how to use hash functions effectively in your PostgreSQL applications.

## Basic Usage

After installing the extension and running `CREATE EXTENSION hashlib;`, you can immediately start using hash functions:

```sql
-- Simple text hashing
SELECT murmurhash3_32('hello world');
-- Result: 1594632942

-- Hash with a custom seed for reproducible randomness
SELECT murmurhash3_32('hello world', 42);
-- Result: 2838467652

-- Fast 64-bit hashing
SELECT wyhash('hello world');
-- Result: -2049815887495798387
```

### Choosing Your First Hash Function

For most use cases, start with one of these recommended functions:

- **xxHash3_64** - Next-generation ultra-fast hash with superior quality
- **WyHash** - Excellent all-around choice, extremely fast with great quality
- **MurmurHash3** - Popular, well-tested, good for general use
- **CityHash64** - Great distribution properties, good for data partitioning

```sql
-- Try each one with the same input to see different outputs
SELECT 
    xxhash3_64('user123') as xxhash3_result,
    wyhash('user123') as wyhash_result,
    murmurhash3_32('user123') as murmur_result,
    cityhash64('user123') as city_result;
```

## Function Signatures

Most hash functions follow similar patterns:

```sql
-- Basic usage (default seed)
SELECT function_name('input');

-- With custom seed
SELECT function_name('input', seed_value);

-- Some functions support multiple seeds
SELECT farmhash64('input', seed1, seed2);
SELECT highwayhash64('input', key1, key2, key3, key4);
```

## Input Types

All hash functions support multiple input types:

```sql
-- Text input
SELECT wyhash('hello world');

-- Bytea input
SELECT wyhash('hello world'::bytea);

-- Integer input
SELECT wyhash(12345);

-- Some functions support bigint
SELECT wyhash(123456789012345::bigint);
```

## Detailed Function Documentation

For comprehensive documentation on each hash function including signatures, parameters, and specific examples, see our [complete algorithm reference](README.md).

The reference includes:
- All function signatures and parameters
- Performance characteristics
- Algorithm-specific features
- Detailed usage examples

---

## Common Use Cases

Here are practical examples of how to use pghashlib in real applications:

### 1. Data Partitioning

Distribute data evenly across multiple partitions or shards.

#### Example: User Data Partitioning

```sql
-- Create sample users table
CREATE TABLE users (
    user_id SERIAL PRIMARY KEY,
    email TEXT,
    name TEXT,
    created_at TIMESTAMP DEFAULT NOW()
);

-- Insert sample data
INSERT INTO users (email, name) VALUES
    ('alice@example.com', 'Alice'),
    ('bob@example.com', 'Bob'),
    ('charlie@example.com', 'Charlie'),
    ('diana@example.com', 'Diana'),
    ('eve@example.com', 'Eve');

-- Partition users into 4 groups using their email
SELECT 
    user_id,
    email,
    abs(cityhash64(email)) % 4 as partition_id
FROM users
ORDER BY partition_id, user_id;
```

#### Why CityHash64 for Partitioning?
- Excellent distribution properties minimize hotspots
- Consistent across different data sizes
- Fast enough for high-throughput applications

#### Advanced Partitioning with 128-bit Hash

For systems with many partitions (1000+), use 128-bit hashes:

```sql
-- Partition into 1024 buckets using the first part of 128-bit hash
SELECT 
    user_id,
    email,
    abs((cityhash128(email))[1]) % 1024 as partition_id
FROM users;
```

### 2. Random Sampling

Create reproducible random samples of your data.

#### Example: A/B Testing User Assignment

```sql
-- Assign users to test groups based on their ID
SELECT 
    user_id,
    email,
    CASE 
        WHEN murmurhash3_32(user_id::text, 12345) % 100 < 50 THEN 'group_a'
        ELSE 'group_b'
    END as test_group
FROM users;
```

#### Example: 10% Sample for Analytics

```sql
-- Get a consistent 10% sample of large table
SELECT * 
FROM large_transactions_table 
WHERE murmurhash3_32(transaction_id::text) % 100 < 10;
```

#### Why Use Seeds for Sampling?
```sql
-- Different seeds create different samples
SELECT COUNT(*) as march_sample 
FROM transactions 
WHERE murmurhash3_32(id::text, 202403) % 100 < 5;  -- March sample

SELECT COUNT(*) as april_sample
FROM transactions 
WHERE murmurhash3_32(id::text, 202404) % 100 < 5;  -- April sample
```

### 3. Data Deduplication

Create fingerprints to identify duplicate or similar records.

#### Example: Basic Deduplication

```sql
-- Create a products table with potential duplicates
CREATE TABLE products (
    product_id SERIAL PRIMARY KEY,
    name TEXT,
    description TEXT,
    price DECIMAL(10,2)
);

-- Create a fingerprint for deduplication
SELECT 
    product_id,
    name,
    price,
    murmurhash3_32(name || description || price::text) as fingerprint
FROM products;

-- Find potential duplicates
WITH fingerprints AS (
    SELECT 
        product_id,
        name,
        murmurhash3_32(name || description || price::text) as fingerprint
    FROM products
)
SELECT fingerprint, array_agg(product_id) as duplicate_ids
FROM fingerprints
GROUP BY fingerprint
HAVING COUNT(*) > 1;
```

#### Strong Deduplication with 128-bit Hashes

For critical applications where collision avoidance is important:

```sql
-- Use CityHash128 for stronger fingerprints
SELECT 
    product_id,
    name,
    cityhash128(name || '|' || description || '|' || price::text) as strong_fingerprint
FROM products;
```

### 4. Consistent Hashing for Caching

Route cache keys consistently to cache servers.

#### Example: Cache Server Selection

```sql
-- Function to select cache server (0-7) for a given key
CREATE OR REPLACE FUNCTION get_cache_server(cache_key TEXT)
RETURNS INTEGER AS $$
BEGIN
    RETURN abs(cityhash64(cache_key)) % 8;
END;
$$ LANGUAGE plpgsql;

-- Usage
SELECT 
    'user:' || user_id as cache_key,
    get_cache_server('user:' || user_id) as cache_server
FROM users;
```

### 5. Security-Sensitive Applications

Use cryptographic hash functions when security matters.

#### Example: Rate Limiting by IP with Hash Flooding Protection

```sql
-- Use SipHash-2-4 to prevent hash flooding attacks
CREATE TABLE rate_limits (
    ip_hash BIGINT,
    request_count INTEGER,
    window_start TIMESTAMP
);

-- Hash IP addresses securely (using custom keys)
INSERT INTO rate_limits (ip_hash, request_count, window_start)
VALUES (
    siphash24('192.168.1.100', 12345, 67890),  -- Custom keys
    1,
    NOW()
);
```

#### Why SipHash for Security?
- Prevents hash flooding DoS attacks
- Cryptographically secure
- Still fast enough for real-time applications

### 6. High-Performance Analytics

Use the fastest hash functions for real-time processing.

#### Example: Real-time Stream Processing

```sql
-- Process high-volume event stream
SELECT 
    event_id,
    user_id,
    -- Ultra-fast partitioning for stream processing
    abs(wyhash(user_id::text)) % 16 as processing_partition,
    -- Fast sampling for monitoring (1% sample)
    (wyhash(event_id::text, 1) % 10000 < 100) as include_in_sample
FROM event_stream
WHERE created_at > NOW() - INTERVAL '1 minute';
```

#### Why WyHash for High Performance?
- Extremely fast (often faster than xxHash3)
- Excellent quality (passes all statistical tests)
- Used by Go, Zig, V, and Nim languages

### 7. Cross-Platform Compatibility

Ensure consistent results across different systems.

#### Example: Distributed System Coordination

```sql
-- Use t1ha1 for consistent results across different architectures
SELECT 
    document_id,
    t1ha1(document_content) as content_hash
FROM documents
WHERE updated_at > NOW() - INTERVAL '1 day';
```

## Performance Tips

1. **Choose the right hash for your workload:**
   - **Fastest**: WyHash, t1ha0, xxHash64
   - **Best distribution**: CityHash64, FarmHash64
   - **Security**: SipHash-2-4, HighwayHash

2. **Use appropriate output sizes:**
   - 32-bit for small partition counts (< 1000)
   - 64-bit for general use
   - 128-bit for large partition counts or strong deduplication

3. **Consider seeds for reproducibility:**
   ```sql
   -- Reproducible randomness across runs
   SELECT * FROM table WHERE murmurhash3_32(id::text, 20241201) % 100 < 10;
   ```

4. **Batch operations when possible:**
   ```sql
   -- Better than row-by-row processing
   UPDATE users 
   SET partition_id = abs(cityhash64(email)) % 8;
   ```

## Next Steps

- Explore the [complete algorithm reference](README.md) for detailed documentation
- Try different hash functions with your data to see performance differences
- Experiment with seeds to create different random samples
- Consider your specific use case requirements (speed vs. security vs. distribution quality)

Remember: hash functions are deterministic - the same input always produces the same output, making them perfect for consistent data processing across your application.