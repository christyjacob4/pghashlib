# pghashlib

pghashlib is a PostgreSQL extension providing high-performance hash functions for data processing and analysis. Currently includes MurmurHash3, CRC32, CityHash64, CityHash128, SipHash-2-4, SpookyHash, xxHash32, xxHash64, FarmHash32, FarmHash64, HighwayHash64, HighwayHash128, HighwayHash256, MetroHash64, MetroHash128, t1ha0, t1ha1, t1ha2, t1ha2_128, WyHash, lookup2, lookup3be, and lookup3le algorithms.

## Table of Contents

1. [Installation](#installation)
2. [Quick Start](#quick-start)
3. [Supported Functions](#supported-functions)
4. [Documentation](#documentation)
5. [Compatibility](#compatibility)
6. [Contributing](#contributing)
7. [License](#license)

## Installation

### Linux and Mac

```bash
git clone https://github.com/christyjacob4/pghashlib.git
cd pghashlib
make
make install # may need sudo
```

Make sure you have PostgreSQL development headers installed:
- **Ubuntu/Debian**: `sudo apt-get install postgresql-server-dev-all`
- **CentOS/RHEL/Fedora**: `sudo yum install postgresql-devel`
- **macOS**: `brew install postgresql`

### Docker

```bash
docker compose up -d --build
docker compose exec postgres psql -U postgres -c "CREATE EXTENSION hashlib;"
```

### Enable Extension

```sql
CREATE EXTENSION hashlib;
```

## Quick Start

```sql
-- Basic usage examples
SELECT murmurhash3_32('hello world');
SELECT cityhash64('hello world');
SELECT wyhash('hello world');

-- With custom seeds
SELECT murmurhash3_32('hello world', 42);
SELECT xxhash64('hello world', 12345);

-- Common use cases
-- Data partitioning
SELECT abs(cityhash64(user_id::text)) % 8 AS partition FROM users;

-- Random sampling (10%)
SELECT * FROM large_table WHERE murmurhash3_32(id::text) % 100 < 10;
```

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

## Documentation

- **[Getting Started Guide](docs/getting-started.md)** - Learn how to use hash functions with practical examples and common use cases
- **[Algorithm Reference](docs/README.md)** - Complete documentation for all hash functions with detailed examples

## Compatibility

- **PostgreSQL Versions**: 12, 13, 14, 15, 16, 17
- **Operating Systems**: Linux, macOS, Windows (via WSL)
- **Architectures**: x86_64, ARM64

## Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for detailed information on development setup, testing, and contribution guidelines.

## License

This project is licensed under the PostgreSQL License - see the [LICENSE](LICENSE) file for details.

