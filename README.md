# pghashlib - high performance hash functions for PostgreSQL

A PostgreSQL extension providing high-performance hash functions for data processing and analysis. Currently includes MurmurHash3, CRC32, CityHash64, CityHash128, lookup2, lookup3be, and lookup3le algorithms.

## Table of Contents

1. [Installation](#installation)
   - [Prerequisites](#prerequisites)
   - [From Source (Recommended)](#from-source-recommended)
   - [From Release Package](#from-release-package)
   - [Using Docker (Development)](#using-docker-development)
   - [Platform-Specific Instructions](#platform-specific-instructions)
     - [Ubuntu/Debian](#ubuntudebian)
     - [CentOS/RHEL/Fedora](#centosrhelfedora)
     - [macOS with Homebrew](#macos-with-homebrew)
     - [Custom PostgreSQL Installation](#custom-postgresql-installation)
2. [Features](#features)
3. [Supported Functions](#supported-functions)
4. [Usage](#usage)
   - [murmurhash3_32](#murmurhash3_32)
   - [crc32](#crc32)
   - [cityhash64](#cityhash64)
   - [cityhash128](#cityhash128)
   - [lookup2](#lookup2)
   - [lookup3be](#lookup3be)
   - [lookup3le](#lookup3le)
   - [Common Use Cases](#common-use-cases)
     - [Data Partitioning](#data-partitioning)
     - [Sampling](#sampling)
     - [Deduplication](#deduplication)
     - [A/B Testing](#ab-testing)
5. [Compatibility](#compatibility)
6. [Contributing](#contributing)
7. [License](#license)
8. [Acknowledgments](#acknowledgments)

## Installation

### Prerequisites

- PostgreSQL 12 or later
- PostgreSQL development headers (`postgresql-server-dev` package)
- GCC or compatible C compiler
- Make

### From Source (Recommended)

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

### From Release Package

1. **Download the latest release:**
   ```bash
   wget https://github.com/yourusername/pghashlib/releases/download/v0.1.0/hashlib-0.1.0.tar.gz
   tar -xzf hashlib-0.1.0.tar.gz
   cd hashlib-0.1.0
   ```

2. **Follow steps 2-4 from above**

### Using Docker (Development)

```bash
docker compose up -d --build
docker compose exec postgres psql -U postgres -c "CREATE EXTENSION hashlib;"
```

### Platform-Specific Instructions

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

#### Custom PostgreSQL Installation

If PostgreSQL is installed in a non-standard location:

```bash
make PG_CONFIG=/path/to/pg_config
sudo make install PG_CONFIG=/path/to/pg_config
```

## Features

- **MurmurHash3**: Fast, non-cryptographic hash function
- **CRC32**: Cyclic redundancy check algorithm for error detection
- **CityHash64**: High-performance 64-bit hash function from Google
- **CityHash128**: High-performance 128-bit hash function from Google
- **lookup2**: Bob Jenkins' lookup2 hash function - fast and well-distributed
- **lookup3be**: Bob Jenkins' lookup3 hash function with big-endian byte order - improved version of lookup2
- **lookup3le**: Bob Jenkins' lookup3 hash function with little-endian byte order - optimized for Intel/AMD systems
- **Multiple Input Types**: Supports `text`, `bytea`, and `integer` inputs
- **Custom Seed Support**: Optional seed parameter for hash customization
- **High Performance**: Optimized C implementation
- **PostgreSQL Integration**: Native PostgreSQL extension with full SQL support

## Supported Functions

| Function | Input Types | Optional Seed | Return Type | Description |
|----------|-------------|---------------|-------------|-------------|
| `murmurhash3_32` | `text`, `bytea`, `integer` | Yes | `integer` | 32-bit MurmurHash3 - fast, non-cryptographic hash |
| `crc32` | `text`, `bytea`, `integer` | Yes | `integer` | 32-bit CRC32 - cyclic redundancy check hash |
| `cityhash64` | `text`, `bytea`, `integer` | Yes | `bigint` | 64-bit CityHash - high-performance hash by Google |
| `cityhash128` | `text`, `bytea`, `integer` | Yes | `bigint[]` | 128-bit CityHash - returns array of two 64-bit values |
| `lookup2` | `text`, `bytea`, `integer` | Yes | `integer` | 32-bit lookup2 - Bob Jenkins' hash function |
| `lookup3be` | `text`, `bytea`, `integer` | Yes | `integer` | 32-bit lookup3be - Bob Jenkins' lookup3 with big-endian order |
| `lookup3le` | `text`, `bytea`, `integer` | Yes | `integer` | 32-bit lookup3le - Bob Jenkins' lookup3 with little-endian order |

## Usage

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

<details>
<summary><strong>Examples</strong></summary>

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

</details>

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

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default seed
SELECT crc32('hello world');
-- Result: CRC32 of text

-- Hash text with custom seed
SELECT crc32('hello world', 42);
-- Result: CRC32 with custom seed

-- Hash bytea data
SELECT crc32('hello'::bytea);
-- Result: CRC32 of bytea data

-- Hash integer
SELECT crc32(12345);
-- Result: CRC32 of integer
```

</details>

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
- Second parameter (optional): Seed value

<details>
<summary><strong>Examples</strong></summary>

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

</details>

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

<details>
<summary><strong>Examples</strong></summary>

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

</details>

### lookup2

lookup2 is Bob Jenkins' hash function, designed for fast hashing with good distribution properties.

**Signatures:**
- `lookup2(text)` → `integer`
- `lookup2(text, integer)` → `integer`
- `lookup2(bytea)` → `integer`
- `lookup2(bytea, integer)` → `integer`
- `lookup2(integer)` → `integer`
- `lookup2(integer, integer)` → `integer`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Initial value/seed (default: 0)

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default initval (0)
SELECT lookup2('hello world');
-- Result: lookup2 hash of text

-- Hash text with custom initval
SELECT lookup2('hello world', 42);
-- Result: lookup2 hash with custom initval

-- Hash bytea data
SELECT lookup2('hello world'::bytea);
-- Result: lookup2 hash of bytea data

-- Hash bytea with custom initval
SELECT lookup2('hello world'::bytea, 42);
-- Result: lookup2 hash with custom initval

-- Hash integer values
SELECT lookup2(12345);
-- Result: lookup2 hash of integer

-- Hash integer with custom initval
SELECT lookup2(12345, 42);
-- Result: lookup2 hash with custom initval
```

</details>

### lookup3be

lookup3be is Bob Jenkins' lookup3 hash function with big-endian byte order, an improved version of lookup2 with better mixing and avalanche properties.

**Signatures:**
- `lookup3be(text)` → `integer`
- `lookup3be(text, integer)` → `integer`
- `lookup3be(bytea)` → `integer`
- `lookup3be(bytea, integer)` → `integer`
- `lookup3be(integer)` → `integer`
- `lookup3be(integer, integer)` → `integer`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Initial value/seed (default: 0)

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default initval (0)
SELECT lookup3be('hello world');
-- Result: lookup3be hash of text

-- Hash text with custom initval
SELECT lookup3be('hello world', 42);
-- Result: lookup3be hash with custom initval

-- Hash bytea data
SELECT lookup3be('hello world'::bytea);
-- Result: lookup3be hash of bytea data

-- Hash bytea with custom initval
SELECT lookup3be('hello world'::bytea, 42);
-- Result: lookup3be hash with custom initval

-- Hash integer values
SELECT lookup3be(12345);
-- Result: lookup3be hash of integer

-- Hash integer with custom initval
SELECT lookup3be(12345, 42);
-- Result: lookup3be hash with custom initval
```

</details>

### lookup3le

lookup3le is Bob Jenkins' lookup3 hash function with little-endian byte order, optimized for Intel/AMD processors and other little-endian systems for maximum performance.

**Signatures:**
- `lookup3le(text)` → `integer`
- `lookup3le(text, integer)` → `integer`
- `lookup3le(bytea)` → `integer`
- `lookup3le(bytea, integer)` → `integer`
- `lookup3le(integer)` → `integer`
- `lookup3le(integer, integer)` → `integer`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Initial value/seed (default: 0)

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default initval (0)
SELECT lookup3le('hello world');
-- Result: lookup3le hash of text

-- Hash text with custom initval
SELECT lookup3le('hello world', 42);
-- Result: lookup3le hash with custom initval

-- Hash bytea data
SELECT lookup3le('hello world'::bytea);
-- Result: lookup3le hash of bytea data

-- Hash bytea with custom initval
SELECT lookup3le('hello world'::bytea, 42);
-- Result: lookup3le hash with custom initval

-- Hash integer values
SELECT lookup3le(12345);
-- Result: lookup3le hash of integer

-- Hash integer with custom initval
SELECT lookup3le(12345, 42);
-- Result: lookup3le hash with custom initval
```

</details>

### Common Use Cases

#### Data Partitioning

<details>
<summary><strong>Examples</strong></summary>

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

</details>

#### Sampling

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Get approximately 10% random sample
SELECT * FROM large_table 
WHERE murmurhash3_32(id::text) % 100 < 10;
```

</details>

#### Deduplication

<details>
<summary><strong>Examples</strong></summary>

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

</details>

#### A/B Testing

<details>
<summary><strong>Examples</strong></summary>

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

</details>

## Compatibility

- **PostgreSQL Versions**: 12, 13, 14, 15, 16, 17
- **Operating Systems**: Linux, macOS, Windows (via WSL)
- **Architectures**: x86_64, ARM64

## Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for detailed information on development setup, testing, and contribution guidelines.

## License

This project is licensed under the PostgreSQL License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- MurmurHash3 algorithm by Austin Appleby
- CityHash algorithm by Google Inc.
- lookup2 and lookup3be algorithms by Bob Jenkins
- PostgreSQL Extension Building Infrastructure (PGXS)
- PostgreSQL Community for guidance and support