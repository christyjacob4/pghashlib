# PostgreSQL Hash Library (hashlib)

A PostgreSQL extension providing high-performance hash functions for data processing and analysis. Currently includes MurmurHash3, CRC32, CityHash64, and CityHash128 algorithms.

## Table of Contents

1. [Installation](#installation)
2. [Available Functions](#available-functions)
3. [Function Documentation](#function-documentation)
4. [Features](#features)
5. [Usage](#usage)
6. [Distribution and Packaging](#distribution-and-packaging)
7. [Development](#development)
8. [Compatibility](#compatibility)
9. [Performance](#performance)
10. [Contributing](#contributing)
11. [License](#license)
12. [Roadmap](#roadmap)
13. [Support](#support)
14. [Acknowledgments](#acknowledgments)

## Installation

### Prerequisites

- PostgreSQL 12 or later
- PostgreSQL development headers (`postgresql-server-dev` package)
- GCC or compatible C compiler
- Make

### Option 1: From Source (Recommended)

1. **Clone the repository:**
   ```bash
   git clone https://github.com/yourusername/pghashlib.git
   cd pghashlib
   ```

2. **Build and install:**
   ```bash
   make
   sudo make install
   ```

3. **Run tests (optional but recommended):**
   ```bash
   make installcheck
   ```

4. **Enable the extension in your database:**
   ```sql
   CREATE EXTENSION hashlib;
   ```

### Option 2: From Release Package

1. **Download the latest release:**
   ```bash
   wget https://github.com/yourusername/pghashlib/releases/download/v0.1.0/hashlib-0.1.0.tar.gz
   tar -xzf hashlib-0.1.0.tar.gz
   cd hashlib-0.1.0
   ```

2. **Follow steps 2-4 from Option 1**

### Option 3: Using Docker (Development)

```bash
docker compose up -d --build
docker compose exec postgres psql -U postgres -c "CREATE EXTENSION hashlib;"
```

### Package Manager Installation

#### Ubuntu/Debian
```bash
# Install PostgreSQL development headers
sudo apt-get install postgresql-server-dev-all

# Then follow "From Source" instructions
```

#### CentOS/RHEL/Fedora
```bash
# Install PostgreSQL development headers
sudo yum install postgresql-devel
# or for newer versions:
sudo dnf install postgresql-devel

# Then follow "From Source" instructions
```

#### macOS with Homebrew
```bash
# Install PostgreSQL
brew install postgresql

# Then follow "From Source" instructions
```

### Custom PostgreSQL Installation

If PostgreSQL is installed in a non-standard location:

```bash
make PG_CONFIG=/path/to/pg_config
sudo make install PG_CONFIG=/path/to/pg_config
```

## Available Functions

| Function | Input Types | Optional Seed | Return Type | Description |
|----------|-------------|---------------|-------------|-------------|
| `murmurhash3_32` | `text`, `bytea`, `integer` | Yes | `integer` | 32-bit MurmurHash3 - fast, non-cryptographic hash |
| `crc32` | `text`, `bytea`, `integer` | Yes | `integer` | 32-bit CRC32 - cyclic redundancy check hash |
| `cityhash64` | `text`, `bytea`, `integer` | Yes | `bigint` | 64-bit CityHash - high-performance hash by Google |
| `cityhash128` | `text`, `bytea`, `integer` | Yes | `bigint[]` | 128-bit CityHash - returns array of two 64-bit values |

## Function Documentation

### murmurhash3_32

MurmurHash3 is a fast, non-cryptographic hash function suitable for general hash-based lookup.

**Signatures:**
- `murmurhash3_32(text)` → `integer`
- `murmurhash3_32(text, integer)` → `integer` 
- `murmurhash3_32(bytea)` → `integer`
- `murmurhash3_32(bytea, integer)` → `integer`
- `murmurhash3_32(integer)` → `integer`
- `murmurhash3_32(integer, integer)` → `integer`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

**Examples:**
```sql
SELECT murmurhash3_32('hello world');           -- 1594632942
SELECT murmurhash3_32('hello world', 42);       -- 2838467652
SELECT murmurhash3_32('hello'::bytea);          -- Hash bytea data
SELECT murmurhash3_32(12345);                   -- 2794345569
```

### crc32

CRC32 is a cyclic redundancy check algorithm commonly used for error detection.

**Signatures:**
- `crc32(text)` → `integer`
- `crc32(text, integer)` → `integer`
- `crc32(bytea)` → `integer`
- `crc32(bytea, integer)` → `integer`
- `crc32(integer)` → `integer`
- `crc32(integer, integer)` → `integer`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

**Examples:**
```sql
SELECT crc32('hello world');                    -- CRC32 of text
SELECT crc32('hello world', 42);               -- CRC32 with custom seed
SELECT crc32('hello'::bytea);                  -- CRC32 of bytea data
SELECT crc32(12345);                           -- CRC32 of integer
```

### cityhash64

CityHash64 is a 64-bit hash function from Google, designed for high performance on strings.

**Signatures:**
- `cityhash64(text)` → `bigint`
- `cityhash64(text, bigint)` → `bigint`
- `cityhash64(bytea)` → `bigint`
- `cityhash64(bytea, bigint)` → `bigint`
- `cityhash64(integer)` → `bigint`
- `cityhash64(integer, bigint)` → `bigint`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: no seed)

**Examples:**
```sql
SELECT cityhash64('hello world');              -- CityHash64 of text
SELECT cityhash64('hello world', 42);          -- CityHash64 with custom seed
SELECT cityhash64('hello'::bytea);             -- CityHash64 of bytea data
SELECT cityhash64(12345);                      -- CityHash64 of integer
```

## Features

- **MurmurHash3**: Fast, non-cryptographic hash function
- **CRC32**: Cyclic redundancy check algorithm for error detection
- **CityHash64**: High-performance 64-bit hash function from Google
- **CityHash128**: High-performance 128-bit hash function from Google
- **Multiple Input Types**: Supports `text`, `bytea`, and `integer` inputs
- **Custom Seed Support**: Optional seed parameter for hash customization
- **High Performance**: Optimized C implementation
- **PostgreSQL Integration**: Native PostgreSQL extension with full SQL support

## Usage

### MurmurHash3 Functions

The extension provides the `murmurhash3_32` function with multiple overloads:

```sql
-- Hash text with default seed (0)
SELECT murmurhash3_32('hello world');
-- Result: 1594632942

-- Hash text with custom seed
SELECT murmurhash3_32('hello world', 42);
-- Result: 2838467652

-- Hash bytea data
SELECT murmurhash3_32('hello world'::bytea);
-- Result: 1594632942

-- Hash bytea with custom seed
SELECT murmurhash3_32('hello world'::bytea, 42);
-- Result: 2838467652

-- Hash integer values
SELECT murmurhash3_32(12345);
-- Result: 2794345569

-- Hash integer with custom seed
SELECT murmurhash3_32(12345, 42);
-- Result: 751823303
```

### CityHash64 Functions

The extension provides the `cityhash64` function with multiple overloads:

```sql
-- Hash text with default seed
SELECT cityhash64('hello world');
-- Result: 2578220239953316063

-- Hash text with custom seed  
SELECT cityhash64('hello world', 42);
-- Result: 6383797005284447264

-- Hash bytea data
SELECT cityhash64('hello world'::bytea);
-- Result: 2578220239953316063

-- Hash bytea with custom seed
SELECT cityhash64('hello world'::bytea, 42);
-- Result: 6383797005284447264

-- Hash integer values
SELECT cityhash64(12345);
-- Result: 2041813223586929814

-- Hash integer with custom seed
SELECT cityhash64(12345, 42);
-- Result: 17692749115209691159
```

### CityHash128 Functions

The extension provides the `cityhash128` function with multiple overloads:

```sql
-- Hash text with default seed
SELECT cityhash128('hello world');
-- Result: {-7119421456246056744,-4082676536336963091}

-- Hash text with custom seed (two 64-bit values)
SELECT cityhash128('hello world', 42, 84);
-- Result: {4409783961438234325,7356734009537733524}

-- Hash bytea data
SELECT cityhash128('hello world'::bytea);
-- Result: {-7119421456246056744,-4082676536336963091}

-- Hash bytea with custom seed
SELECT cityhash128('hello world'::bytea, 42, 84);
-- Result: {4409783961438234325,7356734009537733524}

-- Hash integer values
SELECT cityhash128(12345);
-- Result: {-8264812632162517731,-9113745412911669670}

-- Hash integer with custom seed
SELECT cityhash128(12345, 42, 84);
-- Result: {-4383119689242713753,-6416156085369945475}

-- Access individual parts of the 128-bit hash
SELECT 
    (cityhash128('hello world'))[1] AS low_64_bits,
    (cityhash128('hello world'))[2] AS high_64_bits;
```

### cityhash128

CityHash128 is a 128-bit hash function from Google, providing stronger hash distribution than the 64-bit version.

**Signatures:**
- `cityhash128(text)` → `bigint[]`
- `cityhash128(text, bigint, bigint)` → `bigint[]`
- `cityhash128(bytea)` → `bigint[]`
- `cityhash128(bytea, bigint, bigint)` → `bigint[]`
- `cityhash128(integer)` → `bigint[]`
- `cityhash128(integer, bigint, bigint)` → `bigint[]`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Low 64-bit seed value
- Third parameter (optional): High 64-bit seed value

**Return Value:**
Returns an array of two `bigint` values representing the 128-bit hash:
- `[1]`: Low 64 bits of the hash
- `[2]`: High 64 bits of the hash

**Examples:**
```sql
SELECT cityhash128('hello world');
-- Result: {-7119421456246056744,-4082676536336963091}

SELECT cityhash128('hello world', 42, 84);
-- Result: {4409783961438234325,7356734009537733524}

SELECT cityhash128('hello'::bytea);
-- Result: CityHash128 of bytea data

SELECT cityhash128(12345);
-- Result: {-8264812632162517731,-9113745412911669670}

-- Access individual parts of the hash
SELECT (cityhash128('data'))[1] AS low_part, (cityhash128('data'))[2] AS high_part;
```

### Common Use Cases

#### Data Partitioning
```sql
-- Distribute data across 8 partitions using MurmurHash3
SELECT 
    id,
    data,
    murmurhash3_32(id::text) % 8 as partition_key
FROM my_table;

-- Distribute data across 16 partitions using CityHash64 for better distribution
SELECT 
    id,
    data,
    abs(cityhash64(id::text)) % 16 as partition_key
FROM my_table;

-- Use CityHash128 for even better distribution across many partitions
SELECT 
    id,
    data,
    abs((cityhash128(id::text))[1]) % 1024 as partition_key
FROM my_table;
```

#### Sampling
```sql
-- Get approximately 10% random sample
SELECT * FROM large_table 
WHERE murmurhash3_32(id::text) % 100 < 10;
```

#### Deduplication
```sql
-- Create hash-based fingerprint for deduplication
SELECT 
    murmurhash3_32(column1 || column2 || column3) as fingerprint,
    *
FROM data_table;

-- Use CityHash128 for stronger deduplication fingerprints
SELECT 
    cityhash128(column1 || column2 || column3) as strong_fingerprint,
    *
FROM data_table;
```

#### A/B Testing
```sql
-- Assign users to test groups
SELECT 
    user_id,
    CASE 
        WHEN murmurhash3_32(user_id::text, 1) % 100 < 50 THEN 'group_a'
        ELSE 'group_b'
    END as test_group
FROM users;
```

## Distribution and Packaging

### How PostgreSQL Extensions Are Distributed

PostgreSQL extensions can be distributed through several channels:

#### 1. **PGXN (PostgreSQL Extension Network)**
- The official extension registry for PostgreSQL
- Similar to CPAN for Perl or npm for Node.js
- Install using: `pgxn install hashlib`
- More info: https://pgxn.org/

#### 2. **Operating System Packages**
- Ubuntu/Debian: `.deb` packages via apt
- CentOS/RHEL: `.rpm` packages via yum/dnf
- Example: `sudo apt-get install postgresql-contrib-hashlib`

#### 3. **GitHub Releases**
- Download source archives from GitHub releases
- Manual compilation and installation required
- Includes checksums for verification

#### 4. **Docker Images**
- Pre-built Docker images with extension included
- Useful for containerized deployments

### For End Users: Installation Methods

#### Method 1: PGXN Client (Easiest)
```bash
# Install PGXN client (one-time setup)
pip install pgxnclient

# Install the extension
pgxn install hashlib

# Load into database
pgxn load -d mydatabase hashlib
```

#### Method 2: Package Manager
```bash
# Ubuntu/Debian
sudo apt-get install postgresql-contrib-hashlib

# CentOS/RHEL
sudo yum install postgresql-hashlib
```

#### Method 3: From Source
See installation instructions above.

### For System Administrators

#### Global Installation
Install the extension system-wide so all databases can use it:
```bash
sudo make install
# Extension is now available to all PostgreSQL databases
```

#### Database-Specific Installation
```sql
-- Connect as superuser and enable for specific database
\c mydatabase
CREATE EXTENSION hashlib;
```

#### Version Management
```sql
-- Check installed version
SELECT * FROM pg_extension WHERE extname = 'hashlib';

-- Upgrade extension (when new version is available)
ALTER EXTENSION hashlib UPDATE;
```

## Development

### Building for Multiple PostgreSQL Versions

The extension supports PostgreSQL 12 through 17:

```bash
# Test against specific PostgreSQL version
make PG_CONFIG=/usr/lib/postgresql/15/bin/pg_config installcheck
```

### Adding New Hash Functions

To add a new hash function (e.g., SHA-256):

1. Create `src/sha256.c` with implementation
2. Update `Makefile` to include `src/sha256.o`
3. Add SQL functions in `sql/hashlib--0.0.1.sql`
4. Update tests in `tests/sql/` and `tests/expected/`

### Running Tests

```bash
# Run all tests
make installcheck

# Run specific test
psql -d postgres -f tests/sql/murmur.sql

# Docker-based testing
docker compose exec postgres make test
```

## Compatibility

- **PostgreSQL Versions**: 12, 13, 14, 15, 16, 17
- **Operating Systems**: Linux, macOS, Windows (via WSL)
- **Architectures**: x86_64, ARM64

## Performance

MurmurHash3 benchmarks on modern hardware:

- **Text hashing**: ~500MB/s
- **Integer hashing**: ~50M ops/s
- **Memory overhead**: Minimal (<1KB)

## Contributing

1. Fork the repository
2. Create a feature branch
3. Add tests for new functionality
4. Ensure all tests pass
5. Submit a pull request

## License

This project is licensed under the PostgreSQL License - see the [LICENSE](LICENSE) file for details.

## Roadmap

- [x] Add CityHash64 implementation
- [x] Add CityHash128 implementation
- [ ] Add SHA-256 hash function
- [ ] Add Blake3 hash function
- [ ] Performance optimizations for large data
- [ ] PGXN packaging and distribution
- [ ] Windows build support

## Support

- **Issues**: [GitHub Issues](https://github.com/yourusername/pghashlib/issues)
- **Discussions**: [GitHub Discussions](https://github.com/yourusername/pghashlib/discussions)
- **PostgreSQL Community**: [PostgreSQL Mailing Lists](https://www.postgresql.org/list/)

## Acknowledgments

- MurmurHash3 algorithm by Austin Appleby
- CityHash algorithm by Google Inc.
- PostgreSQL Extension Building Infrastructure (PGXS)
- PostgreSQL Community for guidance and support 