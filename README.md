# pghashlib

pghashlib is a PostgreSQL extension providing high-performance hash functions for data processing and analysis. Currently includes MurmurHash3, CRC32, CityHash64, CityHash128, SipHash-2-4, SpookyHash, xxHash32, xxHash64, FarmHash32, FarmHash64, HighwayHash64, HighwayHash128, HighwayHash256, MetroHash64, MetroHash128, t1ha0, t1ha1, t1ha2, t1ha2_128, WyHash, lookup2, lookup3be, and lookup3le algorithms.

## Table of Contents

1. [Installation](#installation)
   - [Linux and Mac](#linux-and-mac)
   - [Windows](#windows)
   - [Installation Notes - Linux and Mac](#installation-notes---linux-and-mac)
   - [Installation Notes - Windows](#installation-notes---windows)
   - [Docker](#docker)
   - [Enable Extension](#enable-extension)
2. [Features](#features)
3. [Supported Functions](#supported-functions)
4. [Usage](#usage)
   - [murmurhash3_32](#murmurhash3_32)
   - [crc32](#crc32)
   - [cityhash64](#cityhash64)
   - [cityhash128](#cityhash128)
   - [siphash24](#siphash24)
   - [spookyhash64](#spookyhash64)
   - [spookyhash128](#spookyhash128)
   - [xxhash32](#xxhash32)
   - [xxhash64](#xxhash64)
   - [farmhash32](#farmhash32)
   - [farmhash64](#farmhash64)
   - [highwayhash64](#highwayhash64)
   - [highwayhash128](#highwayhash128)
   - [highwayhash256](#highwayhash256)
   - [metrohash64](#metrohash64)
   - [metrohash128](#metrohash128)
   - [t1ha0](#t1ha0)
   - [t1ha1](#t1ha1)
   - [t1ha2](#t1ha2)
   - [t1ha2_128](#t1ha2_128)
   - [wyhash](#wyhash)
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
7. [Roadmap](#roadmap)
8. [License](#license)
9. [Acknowledgments](#acknowledgments)

## Installation

### Linux and Mac

Compile and install the extension (supports PostgreSQL 12+):

```sh
cd /tmp
git clone https://github.com/christyjacob4/pghashlib.git
cd pghashlib
make
make install # may need sudo
```

See the [installation notes](#installation-notes---linux-and-mac) if you run into issues.

You can also install it with [Docker](#docker).

### Windows

Ensure [C++ support in Visual Studio](https://learn.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=msvc-170#download-and-install-the-tools) is installed and run `x64 Native Tools Command Prompt for VS [version]` as administrator. Then use `nmake` to build:

```cmd
set "PGROOT=C:\Program Files\PostgreSQL\17"
cd %TEMP%
git clone https://github.com/christyjacob4/pghashlib.git
cd pghashlib
# Note: Windows build requires additional setup - see installation notes
make
make install
```

See the [installation notes](#installation-notes---windows) if you run into issues.

You can also install it with [Docker](#docker).

## Installation Notes - Linux and Mac

Make sure you have the PostgreSQL development headers installed:

**Ubuntu/Debian:**
```bash
sudo apt-get install postgresql-server-dev-all
```

**CentOS/RHEL/Fedora:**
```bash
sudo yum install postgresql-devel
# or for newer versions:
sudo dnf install postgresql-devel
```

**macOS with Homebrew:**
```bash
brew install postgresql
```

**Custom PostgreSQL Installation:**

If PostgreSQL is installed in a non-standard location:
```bash
make PG_CONFIG=/path/to/pg_config
sudo make install PG_CONFIG=/path/to/pg_config
```

## Installation Notes - Windows

Windows support requires additional setup. The extension has been tested on WSL (Windows Subsystem for Linux) - follow the Linux instructions within your WSL environment.

## Docker

```bash
docker compose up -d --build
docker compose exec postgres psql -U postgres -c "CREATE EXTENSION hashlib;"
```

## Enable Extension

After installation, enable the extension in your database:

```sql
CREATE EXTENSION hashlib;
```

## Features

- **MurmurHash3**: Fast, non-cryptographic hash function
- **CRC32**: Cyclic redundancy check algorithm for error detection
- **CityHash64**: High-performance 64-bit hash function from Google
- **CityHash128**: High-performance 128-bit hash function from Google
- **SipHash-2-4**: Cryptographic hash function designed for hash table protection against hash flooding attacks
- **SpookyHash**: Fast 128-bit hash function by Bob Jenkins, optimized for 64-bit processors with excellent avalanche properties
- **xxHash32**: Extremely fast 32-bit non-cryptographic hash function optimized for speed
- **xxHash64**: Extremely fast 64-bit non-cryptographic hash function optimized for speed
- **FarmHash32**: Google's successor to CityHash with improved distribution properties (32-bit)
- **FarmHash64**: Google's successor to CityHash with improved distribution properties (64-bit)
- **HighwayHash64**: Google's SIMD-optimized keyed hash function (64-bit) - designed for high throughput
- **HighwayHash128**: Google's SIMD-optimized keyed hash function (128-bit) - strong data integrity verification
- **HighwayHash256**: Google's SIMD-optimized keyed hash function (256-bit) - maximum collision resistance
- **MetroHash64**: Fast alternative with excellent avalanche properties (64-bit) - algorithmically generated for performance
- **MetroHash128**: Fast alternative with excellent avalanche properties (128-bit) - strong statistical profile similar to MD5
- **t1ha0**: Fastest available t1ha variant - automatically selects optimal implementation for current CPU
- **t1ha1**: Baseline portable t1ha hash - stable across architectures with reasonable quality
- **t1ha2**: Recommended t1ha variant - good quality for checksums and hash tables, optimized for 64-bit systems
- **t1ha2_128**: 128-bit version of t1ha2 - provides extended hash length for collision resistance
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
| `siphash24` | `text`, `bytea`, `integer` | Yes (2 seeds) | `bigint` | 64-bit SipHash-2-4 - cryptographic hash function |
| `spookyhash64` | `text`, `bytea`, `integer` | Yes | `bigint` | 64-bit SpookyHash - fast hash optimized for 64-bit processors |
| `spookyhash128` | `text`, `bytea`, `integer` | Yes (2 seeds) | `bigint[]` | 128-bit SpookyHash - returns array of two 64-bit values |
| `xxhash32` | `text`, `bytea`, `integer` | Yes | `integer` | 32-bit xxHash - extremely fast non-cryptographic hash |
| `xxhash64` | `text`, `bytea`, `integer` | Yes | `bigint` | 64-bit xxHash - extremely fast non-cryptographic hash |
| `farmhash32` | `text`, `bytea`, `integer` | Yes | `integer` | 32-bit FarmHash - Google's successor to CityHash |
| `farmhash64` | `text`, `bytea`, `integer` | Yes (2 seeds) | `bigint` | 64-bit FarmHash - Google's successor to CityHash |
| `highwayhash64` | `text`, `bytea`, `integer` | Yes (4 keys) | `bigint` | 64-bit HighwayHash - Google's SIMD-optimized keyed hash |
| `highwayhash128` | `text`, `bytea`, `integer` | Yes (4 keys) | `bigint[]` | 128-bit HighwayHash - returns array of two 64-bit values |
| `highwayhash256` | `text`, `bytea`, `integer` | Yes (4 keys) | `bigint[]` | 256-bit HighwayHash - returns array of four 64-bit values |
| `metrohash64` | `text`, `bytea`, `integer` | Yes | `bigint` | 64-bit MetroHash - fast alternative with excellent avalanche properties |
| `metrohash128` | `text`, `bytea`, `integer` | Yes | `bigint[]` | 128-bit MetroHash - returns array of two 64-bit values |
| `t1ha0` | `text`, `bytea`, `integer` | Yes | `bigint` | 64-bit t1ha0 - fastest available t1ha variant for current CPU |
| `t1ha1` | `text`, `bytea`, `integer` | Yes | `bigint` | 64-bit t1ha1 - baseline portable hash with stable results |
| `t1ha2` | `text`, `bytea`, `integer` | Yes | `bigint` | 64-bit t1ha2 - recommended variant optimized for 64-bit systems |
| `t1ha2_128` | `text`, `bytea`, `integer` | Yes | `bigint[]` | 128-bit t1ha2 - returns array of two 64-bit values |
| `wyhash` | `text`, `bytea`, `integer`, `bigint` | Yes | `bigint` | 64-bit WyHash - extremely fast quality hash used by Go, Zig, V, Nim |
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

### siphash24

SipHash-2-4 is a cryptographic hash function designed to provide strong protection against hash flooding attacks while maintaining good performance. It uses a 128-bit key for secure hashing.

**Signatures:**
- `siphash24(text)` → `bigint`
- `siphash24(text, integer, integer)` → `bigint`
- `siphash24(bytea)` → `bigint`
- `siphash24(bytea, integer, integer)` → `bigint`
- `siphash24(integer)` → `bigint`
- `siphash24(integer, integer, integer)` → `bigint`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): First 32-bit seed value for 128-bit key
- Third parameter (optional): Second 32-bit seed value for 128-bit key

**Note:** SipHash-2-4 requires a 128-bit key for security. When using custom seeds, provide both 32-bit values. If no seeds are provided, a default key is used.

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default key
SELECT siphash24('hello world');
-- Result: Secure hash with default key

-- Hash text with custom 128-bit key (two 32-bit seeds)
SELECT siphash24('hello world', 42, 84);
-- Result: Secure hash with custom key

-- Hash bytea data
SELECT siphash24('hello world'::bytea);
-- Result: Secure hash of bytea data

-- Hash bytea with custom key
SELECT siphash24('hello world'::bytea, 42, 84);
-- Result: Secure hash with custom key

-- Hash integer values
SELECT siphash24(12345);
-- Result: Secure hash of integer

-- Hash integer with custom key
SELECT siphash24(12345, 42, 84);
-- Result: Secure hash with custom key
```

</details>

### spookyhash64

SpookyHash64 is a 64-bit hash function by Bob Jenkins, optimized for speed on 64-bit processors with excellent avalanche properties.

**Signatures:**
- `spookyhash64(text)` → `bigint`
- `spookyhash64(text, bigint)` → `bigint`
- `spookyhash64(bytea)` → `bigint`
- `spookyhash64(bytea, bigint)` → `bigint`
- `spookyhash64(integer)` → `bigint`
- `spookyhash64(integer, bigint)` → `bigint`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default seed
SELECT spookyhash64('hello world');
-- Result: Fast 64-bit hash

-- Hash text with custom seed
SELECT spookyhash64('hello world', 42);
-- Result: Fast 64-bit hash with custom seed

-- Hash bytea data
SELECT spookyhash64('hello world'::bytea);
-- Result: Fast 64-bit hash of bytea data

-- Hash bytea with custom seed
SELECT spookyhash64('hello world'::bytea, 42);
-- Result: Fast 64-bit hash with custom seed

-- Hash integer values
SELECT spookyhash64(12345);
-- Result: Fast 64-bit hash of integer

-- Hash integer with custom seed
SELECT spookyhash64(12345, 42);
-- Result: Fast 64-bit hash with custom seed
```

</details>

### spookyhash128

SpookyHash128 is a 128-bit hash function by Bob Jenkins, providing stronger hash distribution with two 64-bit output values.

**Signatures:**
- `spookyhash128(text)` → `bigint[]`
- `spookyhash128(text, bigint, bigint)` → `bigint[]`
- `spookyhash128(bytea)` → `bigint[]`
- `spookyhash128(bytea, bigint, bigint)` → `bigint[]`
- `spookyhash128(integer)` → `bigint[]`
- `spookyhash128(integer, bigint, bigint)` → `bigint[]`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): First 64-bit seed value
- Third parameter (optional): Second 64-bit seed value

**Return Value:**
Returns an array of two `bigint` values representing the 128-bit hash:
- `[1]`: First 64 bits of the hash
- `[2]`: Second 64 bits of the hash

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default seed
SELECT spookyhash128('hello world');
-- Result: {first_64_bits, second_64_bits}

-- Hash text with custom seeds (two 64-bit values)
SELECT spookyhash128('hello world', 42, 84);
-- Result: {first_64_bits, second_64_bits}

-- Hash bytea data
SELECT spookyhash128('hello world'::bytea);
-- Result: {first_64_bits, second_64_bits}

-- Hash bytea with custom seeds
SELECT spookyhash128('hello world'::bytea, 42, 84);
-- Result: {first_64_bits, second_64_bits}

-- Hash integer values
SELECT spookyhash128(12345);
-- Result: {first_64_bits, second_64_bits}

-- Hash integer with custom seeds
SELECT spookyhash128(12345, 42, 84);
-- Result: {first_64_bits, second_64_bits}

-- Access individual parts of the 128-bit hash
SELECT 
    (spookyhash128('hello world'))[1] AS first_64_bits,
    (spookyhash128('hello world'))[2] AS second_64_bits;
```

</details>

### farmhash32

FarmHash32 is Google's 32-bit successor to CityHash, designed for better distribution properties while maintaining high performance.

**Signatures:**
- `farmhash32(text)` → `integer`
- `farmhash32(text, integer)` → `integer`
- `farmhash32(bytea)` → `integer`
- `farmhash32(bytea, integer)` → `integer`
- `farmhash32(integer)` → `integer`
- `farmhash32(integer, integer)` → `integer`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default seed
SELECT farmhash32('hello world');
-- Result: Fast 32-bit FarmHash

-- Hash text with custom seed
SELECT farmhash32('hello world', 42);
-- Result: Fast 32-bit FarmHash with custom seed

-- Hash bytea data
SELECT farmhash32('hello world'::bytea);
-- Result: Fast 32-bit FarmHash of bytea data

-- Hash bytea with custom seed
SELECT farmhash32('hello world'::bytea, 42);
-- Result: Fast 32-bit FarmHash with custom seed

-- Hash integer values
SELECT farmhash32(12345);
-- Result: Fast 32-bit FarmHash of integer

-- Hash integer with custom seed
SELECT farmhash32(12345, 42);
-- Result: Fast 32-bit FarmHash with custom seed
```

</details>

### farmhash64

FarmHash64 is Google's 64-bit successor to CityHash, designed for better distribution properties with support for dual seed hashing.

**Signatures:**
- `farmhash64(text)` → `bigint`
- `farmhash64(text, bigint)` → `bigint`
- `farmhash64(text, bigint, bigint)` → `bigint`
- `farmhash64(bytea)` → `bigint`
- `farmhash64(bytea, bigint)` → `bigint`
- `farmhash64(bytea, bigint, bigint)` → `bigint`
- `farmhash64(integer)` → `bigint`
- `farmhash64(integer, bigint)` → `bigint`
- `farmhash64(integer, bigint, bigint)` → `bigint`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): First seed value (default: 0)
- Third parameter (optional): Second seed value for dual seed hashing

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default seed
SELECT farmhash64('hello world');
-- Result: Fast 64-bit FarmHash

-- Hash text with custom seed
SELECT farmhash64('hello world', 42);
-- Result: Fast 64-bit FarmHash with custom seed

-- Hash text with two seeds for stronger customization
SELECT farmhash64('hello world', 42, 84);
-- Result: Fast 64-bit FarmHash with dual seeds

-- Hash bytea data
SELECT farmhash64('hello world'::bytea);
-- Result: Fast 64-bit FarmHash of bytea data

-- Hash bytea with custom seed
SELECT farmhash64('hello world'::bytea, 42);
-- Result: Fast 64-bit FarmHash with custom seed

-- Hash bytea with two seeds
SELECT farmhash64('hello world'::bytea, 42, 84);
-- Result: Fast 64-bit FarmHash with dual seeds

-- Hash integer values
SELECT farmhash64(12345);
-- Result: Fast 64-bit FarmHash of integer

-- Hash integer with custom seed
SELECT farmhash64(12345, 42);
-- Result: Fast 64-bit FarmHash with custom seed

-- Hash integer with two seeds
SELECT farmhash64(12345, 42, 84);
-- Result: Fast 64-bit FarmHash with dual seeds
```

</details>

### highwayhash64

HighwayHash64 is Google's SIMD-optimized keyed hash function providing 64-bit output. It uses a 256-bit key and is designed for high-throughput hashing with cryptographic-level security.

**Signatures:**
- `highwayhash64(text)` → `bigint`
- `highwayhash64(text, bigint, bigint, bigint, bigint)` → `bigint`
- `highwayhash64(bytea)` → `bigint`
- `highwayhash64(bytea, bigint, bigint, bigint, bigint)` → `bigint`
- `highwayhash64(integer)` → `bigint`
- `highwayhash64(integer, bigint, bigint, bigint, bigint)` → `bigint`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Key parameters (optional): Four 64-bit values forming a 256-bit key (default key used if not provided)

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default key
SELECT highwayhash64('hello world');
-- Result: SIMD-optimized 64-bit hash

-- Hash text with custom key (4 bigint values)
SELECT highwayhash64('hello world', 1, 2, 3, 4);
-- Result: Keyed hash with custom 256-bit key

-- Hash bytea data
SELECT highwayhash64('hello world'::bytea);
-- Result: SIMD-optimized hash of bytea data

-- Hash bytea with custom key
SELECT highwayhash64('hello world'::bytea, 42, 84, 168, 336);
-- Result: Keyed hash with custom key

-- Hash integer values
SELECT highwayhash64(12345);
-- Result: SIMD-optimized hash of integer

-- Hash integer with custom key
SELECT highwayhash64(12345, 1, 2, 3, 4);
-- Result: Keyed hash with custom key
```

</details>

### highwayhash128

HighwayHash128 is Google's SIMD-optimized keyed hash function providing 128-bit output for stronger collision resistance and data integrity verification.

**Signatures:**
- `highwayhash128(text)` → `bigint[]`
- `highwayhash128(text, bigint, bigint, bigint, bigint)` → `bigint[]`
- `highwayhash128(bytea)` → `bigint[]`
- `highwayhash128(bytea, bigint, bigint, bigint, bigint)` → `bigint[]`
- `highwayhash128(integer)` → `bigint[]`
- `highwayhash128(integer, bigint, bigint, bigint, bigint)` → `bigint[]`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Key parameters (optional): Four 64-bit values forming a 256-bit key (default key used if not provided)

**Return Value:**
Returns an array of two `bigint` values representing the 128-bit hash:
- `[1]`: First 64 bits of the hash
- `[2]`: Second 64 bits of the hash

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default key
SELECT highwayhash128('hello world');
-- Result: {first_64_bits, second_64_bits}

-- Hash text with custom key
SELECT highwayhash128('hello world', 1, 2, 3, 4);
-- Result: Keyed 128-bit hash

-- Hash bytea data
SELECT highwayhash128('hello world'::bytea);
-- Result: SIMD-optimized 128-bit hash

-- Hash bytea with custom key
SELECT highwayhash128('hello world'::bytea, 42, 84, 168, 336);
-- Result: Keyed 128-bit hash

-- Hash integer values
SELECT highwayhash128(12345);
-- Result: SIMD-optimized 128-bit hash

-- Hash integer with custom key
SELECT highwayhash128(12345, 1, 2, 3, 4);
-- Result: Keyed 128-bit hash

-- Access individual parts of the 128-bit hash
SELECT 
    (highwayhash128('hello world'))[1] AS first_64_bits,
    (highwayhash128('hello world'))[2] AS second_64_bits;
```

</details>

### highwayhash256

HighwayHash256 is Google's SIMD-optimized keyed hash function providing 256-bit output for maximum collision resistance and cryptographic applications.

**Signatures:**
- `highwayhash256(text)` → `bigint[]`
- `highwayhash256(text, bigint, bigint, bigint, bigint)` → `bigint[]`
- `highwayhash256(bytea)` → `bigint[]`
- `highwayhash256(bytea, bigint, bigint, bigint, bigint)` → `bigint[]`
- `highwayhash256(integer)` → `bigint[]`
- `highwayhash256(integer, bigint, bigint, bigint, bigint)` → `bigint[]`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Key parameters (optional): Four 64-bit values forming a 256-bit key (default key used if not provided)

**Return Value:**
Returns an array of four `bigint` values representing the 256-bit hash:
- `[1]`: First 64 bits of the hash
- `[2]`: Second 64 bits of the hash
- `[3]`: Third 64 bits of the hash
- `[4]`: Fourth 64 bits of the hash

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default key
SELECT highwayhash256('hello world');
-- Result: {first_64_bits, second_64_bits, third_64_bits, fourth_64_bits}

-- Hash text with custom key
SELECT highwayhash256('hello world', 1, 2, 3, 4);
-- Result: Keyed 256-bit hash

-- Hash bytea data
SELECT highwayhash256('hello world'::bytea);
-- Result: SIMD-optimized 256-bit hash

-- Hash bytea with custom key
SELECT highwayhash256('hello world'::bytea, 42, 84, 168, 336);
-- Result: Keyed 256-bit hash

-- Hash integer values
SELECT highwayhash256(12345);
-- Result: SIMD-optimized 256-bit hash

-- Hash integer with custom key
SELECT highwayhash256(12345, 1, 2, 3, 4);
-- Result: Keyed 256-bit hash

-- Access individual parts of the 256-bit hash
SELECT 
    (highwayhash256('hello world'))[1] AS first_64_bits,
    (highwayhash256('hello world'))[2] AS second_64_bits,
    (highwayhash256('hello world'))[3] AS third_64_bits,
    (highwayhash256('hello world'))[4] AS fourth_64_bits;
```

</details>

### metrohash64

MetroHash64 is a fast alternative hash function with excellent avalanche properties. It's algorithmically generated for performance and provides a good balance of speed and distribution quality.

**Signatures:**
- `metrohash64(text)` → `bigint`
- `metrohash64(text, bigint)` → `bigint`
- `metrohash64(bytea)` → `bigint`
- `metrohash64(bytea, bigint)` → `bigint`
- `metrohash64(integer)` → `bigint`
- `metrohash64(integer, bigint)` → `bigint`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default seed
SELECT metrohash64('hello world');
-- Result: Fast 64-bit MetroHash

-- Hash text with custom seed
SELECT metrohash64('hello world', 42);
-- Result: Fast 64-bit MetroHash with custom seed

-- Hash bytea data
SELECT metrohash64('hello world'::bytea);
-- Result: Fast 64-bit MetroHash of bytea data

-- Hash bytea with custom seed
SELECT metrohash64('hello world'::bytea, 42);
-- Result: Fast 64-bit MetroHash with custom seed

-- Hash integer values
SELECT metrohash64(12345);
-- Result: Fast 64-bit MetroHash of integer

-- Hash integer with custom seed
SELECT metrohash64(12345, 42);
-- Result: Fast 64-bit MetroHash with custom seed
```

</details>

### metrohash128

MetroHash128 is a fast alternative hash function providing 128-bit output with excellent avalanche properties and a strong statistical profile similar to MD5 but much faster.

**Signatures:**
- `metrohash128(text)` → `bigint[]`
- `metrohash128(text, bigint)` → `bigint[]`
- `metrohash128(bytea)` → `bigint[]`
- `metrohash128(bytea, bigint)` → `bigint[]`
- `metrohash128(integer)` → `bigint[]`
- `metrohash128(integer, bigint)` → `bigint[]`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

**Return Value:**
Returns an array of two `bigint` values representing the 128-bit hash:
- `[1]`: First 64 bits of the hash
- `[2]`: Second 64 bits of the hash

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default seed
SELECT metrohash128('hello world');
-- Result: {first_64_bits, second_64_bits}

-- Hash text with custom seed
SELECT metrohash128('hello world', 42);
-- Result: Fast 128-bit MetroHash with custom seed

-- Hash bytea data
SELECT metrohash128('hello world'::bytea);
-- Result: Fast 128-bit MetroHash of bytea data

-- Hash bytea with custom seed
SELECT metrohash128('hello world'::bytea, 42);
-- Result: Fast 128-bit MetroHash with custom seed

-- Hash integer values
SELECT metrohash128(12345);
-- Result: Fast 128-bit MetroHash of integer

-- Hash integer with custom seed
SELECT metrohash128(12345, 42);
-- Result: Fast 128-bit MetroHash with custom seed

-- Access individual parts of the 128-bit hash
SELECT 
    (metrohash128('hello world'))[1] AS first_64_bits,
    (metrohash128('hello world'))[2] AS second_64_bits;
```

</details>

### t1ha0

t1ha0 is the fastest available variant of the t1ha (Fast Positive Hash) family. It automatically selects the optimal implementation for the current CPU architecture, prioritizing speed over consistent cross-platform results.

**Signatures:**
- `t1ha0(text)` → `bigint`
- `t1ha0(text, bigint)` → `bigint`
- `t1ha0(bytea)` → `bigint`
- `t1ha0(bytea, bigint)` → `bigint`
- `t1ha0(integer)` → `bigint`
- `t1ha0(integer, bigint)` → `bigint`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default seed
SELECT t1ha0('hello world');
-- Result: Ultra-fast 64-bit t1ha0 hash

-- Hash text with custom seed
SELECT t1ha0('hello world', 42);
-- Result: Ultra-fast 64-bit t1ha0 hash with custom seed

-- Hash bytea data
SELECT t1ha0('hello world'::bytea);
-- Result: Ultra-fast 64-bit t1ha0 hash of bytea data

-- Hash integer values
SELECT t1ha0(12345);
-- Result: Ultra-fast 64-bit t1ha0 hash of integer
```

</details>

### t1ha1

t1ha1 is the baseline portable variant of t1ha, providing stable results across different architectures with reasonable quality for checksums and hash tables.

**Signatures:**
- `t1ha1(text)` → `bigint`
- `t1ha1(text, bigint)` → `bigint`
- `t1ha1(bytea)` → `bigint`
- `t1ha1(bytea, bigint)` → `bigint`
- `t1ha1(integer)` → `bigint`
- `t1ha1(integer, bigint)` → `bigint`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default seed
SELECT t1ha1('hello world');
-- Result: Portable 64-bit t1ha1 hash

-- Hash text with custom seed
SELECT t1ha1('hello world', 42);
-- Result: Portable 64-bit t1ha1 hash with custom seed

-- Hash bytea data
SELECT t1ha1('hello world'::bytea);
-- Result: Portable 64-bit t1ha1 hash of bytea data

-- Hash integer values
SELECT t1ha1(12345);
-- Result: Portable 64-bit t1ha1 hash of integer
```

</details>

### t1ha2

t1ha2 is the recommended variant of t1ha, providing good quality for checksums and hash tables while being optimized for 64-bit systems. It offers the best balance of speed and quality.

**Signatures:**
- `t1ha2(text)` → `bigint`
- `t1ha2(text, bigint)` → `bigint`
- `t1ha2(bytea)` → `bigint`
- `t1ha2(bytea, bigint)` → `bigint`
- `t1ha2(integer)` → `bigint`
- `t1ha2(integer, bigint)` → `bigint`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default seed
SELECT t1ha2('hello world');
-- Result: High-quality 64-bit t1ha2 hash

-- Hash text with custom seed
SELECT t1ha2('hello world', 42);
-- Result: High-quality 64-bit t1ha2 hash with custom seed

-- Hash bytea data
SELECT t1ha2('hello world'::bytea);
-- Result: High-quality 64-bit t1ha2 hash of bytea data

-- Hash integer values
SELECT t1ha2(12345);
-- Result: High-quality 64-bit t1ha2 hash of integer
```

</details>

### t1ha2_128

t1ha2_128 is the 128-bit version of t1ha2, providing extended hash length for applications requiring higher collision resistance while maintaining the performance characteristics of t1ha2.

**Signatures:**
- `t1ha2_128(text)` → `bigint[]`
- `t1ha2_128(text, bigint)` → `bigint[]`
- `t1ha2_128(bytea)` → `bigint[]`
- `t1ha2_128(bytea, bigint)` → `bigint[]`
- `t1ha2_128(integer)` → `bigint[]`
- `t1ha2_128(integer, bigint)` → `bigint[]`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

**Return Value:**
Returns an array of two `bigint` values representing the 128-bit hash:
- `[1]`: First 64 bits of the hash
- `[2]`: Second 64 bits of the hash

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default seed
SELECT t1ha2_128('hello world');
-- Result: {first_64_bits, second_64_bits}

-- Hash text with custom seed
SELECT t1ha2_128('hello world', 42);
-- Result: High-quality 128-bit t1ha2 hash with custom seed

-- Hash bytea data
SELECT t1ha2_128('hello world'::bytea);
-- Result: High-quality 128-bit t1ha2 hash of bytea data

-- Hash integer values
SELECT t1ha2_128(12345);
-- Result: High-quality 128-bit t1ha2 hash of integer

-- Access individual parts of the 128-bit hash
SELECT 
    (t1ha2_128('hello world'))[1] AS first_64_bits,
    (t1ha2_128('hello world'))[2] AS second_64_bits;
```

</details>

### wyhash

<details>
<summary>Click to expand wyhash usage examples</summary>

WyHash is one of the fastest quality hash functions available, designed by Wang Yi. It offers exceptional performance while maintaining excellent hash quality and is used as the default hash algorithm in Go (since 1.17), Zig, V, and Nim languages.

**Key Features:**
- Extremely fast performance, often faster than xxHash3 and t1ha
- Excellent hash quality, passes all SMHasher tests
- Optimized for modern 64-bit systems
- Particularly efficient with short keys
- Public domain license (The Unlicense)

**Signatures:**
- `wyhash(text)` → `bigint`
- `wyhash(text, bigint)` → `bigint`
- `wyhash(bytea)` → `bigint`
- `wyhash(bytea, bigint)` → `bigint`
- `wyhash(integer)` → `bigint`
- `wyhash(integer, bigint)` → `bigint`
- `wyhash(bigint)` → `bigint`
- `wyhash(bigint, bigint)` → `bigint`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, `integer`, or `bigint`)
- Second parameter (optional): Seed value (default: 0)

**Return Value:**
Returns a 64-bit signed integer (`bigint`) hash value.

**Usage Examples:**
```sql
-- Hash text data
SELECT wyhash('hello world');
-- Result: Fast, high-quality WyHash of text

-- Hash text with custom seed
SELECT wyhash('hello world', 42);
-- Result: Fast, high-quality WyHash with custom seed

-- Hash bytea data
SELECT wyhash('hello world'::bytea);
-- Result: Fast, high-quality WyHash of bytea data

-- Hash integer values
SELECT wyhash(12345);
-- Result: Fast, high-quality WyHash of integer

-- Hash bigint values
SELECT wyhash(123456789012345::bigint);
-- Result: Fast, high-quality WyHash of bigint value

-- Use in data partitioning (extremely fast)
SELECT 
    user_id,
    abs(wyhash(user_id::text) % 10) as partition
FROM users;

-- Use for sampling (high performance)
SELECT * FROM large_table 
WHERE abs(wyhash(id::text) % 100) < 5;  -- 5% sample
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

### xxhash32

xxHash32 is an extremely fast 32-bit non-cryptographic hash function optimized for speed while maintaining good distribution properties.

**Signatures:**
- `xxhash32(text)` → `integer`
- `xxhash32(text, integer)` → `integer`
- `xxhash32(bytea)` → `integer`
- `xxhash32(bytea, integer)` → `integer`
- `xxhash32(integer)` → `integer`
- `xxhash32(integer, integer)` → `integer`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default seed
SELECT xxhash32('hello world');
-- Result: Fast 32-bit hash

-- Hash text with custom seed
SELECT xxhash32('hello world', 42);
-- Result: Fast 32-bit hash with custom seed

-- Hash bytea data
SELECT xxhash32('hello world'::bytea);
-- Result: Fast 32-bit hash of bytea data

-- Hash bytea with custom seed
SELECT xxhash32('hello world'::bytea, 42);
-- Result: Fast 32-bit hash with custom seed

-- Hash integer values
SELECT xxhash32(12345);
-- Result: Fast 32-bit hash of integer

-- Hash integer with custom seed
SELECT xxhash32(12345, 42);
-- Result: Fast 32-bit hash with custom seed
```

</details>

### xxhash64

xxHash64 is an extremely fast 64-bit non-cryptographic hash function optimized for speed on 64-bit processors while maintaining excellent distribution properties.

**Signatures:**
- `xxhash64(text)` → `bigint`
- `xxhash64(text, bigint)` → `bigint`
- `xxhash64(bytea)` → `bigint`
- `xxhash64(bytea, bigint)` → `bigint`
- `xxhash64(integer)` → `bigint`
- `xxhash64(integer, bigint)` → `bigint`

**Parameters:**
- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

<details>
<summary><strong>Examples</strong></summary>

```sql
-- Hash text with default seed
SELECT xxhash64('hello world');
-- Result: Fast 64-bit hash

-- Hash text with custom seed
SELECT xxhash64('hello world', 42);
-- Result: Fast 64-bit hash with custom seed

-- Hash bytea data
SELECT xxhash64('hello world'::bytea);
-- Result: Fast 64-bit hash of bytea data

-- Hash bytea with custom seed
SELECT xxhash64('hello world'::bytea, 42);
-- Result: Fast 64-bit hash with custom seed

-- Hash integer values
SELECT xxhash64(12345);
-- Result: Fast 64-bit hash of integer

-- Hash integer with custom seed
SELECT xxhash64(12345, 42);
-- Result: Fast 64-bit hash with custom seed
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

## Roadmap

Additional non-cryptographic hash functions planned for future releases:

### **High Priority**
- [x] **xxHash** (xxh32, xxh64) - Extremely fast general-purpose hashing
- [x] **FarmHash** - Google's successor to CityHash with better distribution
- [x] **HighwayHash** - SIMD-optimized keyed hash function

### **Medium Priority**
- [x] **MetroHash** - Fast alternative with good avalanche properties
- [x] **t1ha** - Fast Positive Hash optimized for x86-64
- [x] **wyhash** - Simple, fast implementation

### **Specialized**
- [ ] **Adler-32** - Fast checksum (used in zlib)
- [ ] **Rabin-Karp** - Rolling hash for sliding windows
- [ ] **Fletcher's Checksum** - Alternative error detection
- [ ] **Pearson Hashing** - Simple hash for small data

## License

This project is licensed under the PostgreSQL License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- MurmurHash3 algorithm by Austin Appleby
- CityHash algorithm by Google Inc.
- SipHash-2-4 algorithm by Jean-Philippe Aumasson and Daniel J. Bernstein
- SpookyHash algorithm by Bob Jenkins
- xxHash algorithm by Yann Collet
- FarmHash algorithm by Google Inc.
- HighwayHash algorithm by Google Inc.
- MetroHash algorithm by J. Andrew Rogers
- lookup2 and lookup3be algorithms by Bob Jenkins
- PostgreSQL Extension Building Infrastructure (PGXS)
- PostgreSQL Community for guidance and support