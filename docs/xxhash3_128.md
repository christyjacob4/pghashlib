# xxHash3_128

xxHash3_128 is the 128-bit variant of the next-generation xxHash3 algorithm, providing excellent hash quality and performance for applications requiring 128-bit hash values.

## Signatures

- `xxhash3_128(text)` → `text`
- `xxhash3_128(text, bigint)` → `text`
- `xxhash3_128(bytea)` → `text`
- `xxhash3_128(bytea, bigint)` → `text`
- `xxhash3_128(integer)` → `text`
- `xxhash3_128(integer, bigint)` → `text`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

## Return Value

Returns a 32-character hexadecimal string representing the 128-bit hash value (high64:low64 format).

## Examples

```sql
-- Hash text with default seed
SELECT xxhash3_128('hello world');
-- Result: '4fcb4334814eda5c31e6739c7f70a8b9' (example output)

-- Hash text with custom seed
SELECT xxhash3_128('hello world', 42);
-- Result: Different 128-bit hash with custom seed

-- Hash bytea data
SELECT xxhash3_128('hello world'::bytea);
-- Result: 128-bit hash of bytea data

-- Hash bytea with custom seed
SELECT xxhash3_128('hello world'::bytea, 42);
-- Result: Seeded 128-bit hash of bytea data

-- Hash integer values
SELECT xxhash3_128(12345);
-- Result: 128-bit hash of integer

-- Hash integer with custom seed
SELECT xxhash3_128(12345, 42);
-- Result: Seeded 128-bit hash of integer

-- Use in applications requiring high-quality hashing
SELECT xxhash3_128(user_data) AS hash_id
FROM sensitive_table
WHERE xxhash3_128(user_data) = 'a1b2c3d4e5f6789012345678901234ab';
```

## Features

- **128-bit Output**: Provides twice the hash space of 64-bit algorithms
- **Superior Hash Quality**: Excellent dispersion properties with minimal collisions
- **High Performance**: Competitive speed despite larger output size
- **Cryptographic Strength**: While not cryptographically secure, offers strong resistance to hash attacks
- **Consistent Results**: Same input produces identical hash across platforms

## Algorithm Details

xxHash3_128 extends the xxHash3 design to produce 128-bit hashes:

- Uses the same adaptive size-based strategies as xxHash3_64
- Employs 64x64→128 multiplication for enhanced mixing
- Applies dual avalanche functions for both high/low 64-bit components
- Maintains high performance through vectorization optimizations

## Use Cases

- **UUID Generation**: High-quality pseudo-random identifiers
- **Cryptographic Applications**: Non-cryptographic use cases requiring strong hashing
- **Large-Scale Data Processing**: Applications with billions of records requiring collision resistance
- **Content Addressable Storage**: Systems using hash values as content identifiers
- **Distributed Systems**: Consistent hashing across multiple nodes
- **Data Deduplication**: Identifying duplicate content with high confidence

## Performance Characteristics

- Faster than most 128-bit hash functions
- Scales well with input size
- Benefits from modern CPU features (SIMD, 64-bit arithmetic)
- Maintains consistent performance across different input patterns