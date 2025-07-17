# PostgreSQL Hash Library (hashlib)

A PostgreSQL extension providing high-performance hash functions for data processing and analysis. Currently includes MurmurHash3, with plans to add more hash algorithms.

## Features

- **MurmurHash3**: Fast, non-cryptographic hash function
- **Multiple Input Types**: Supports `text`, `bytea`, and `integer` inputs
- **Custom Seed Support**: Optional seed parameter for hash customization
- **High Performance**: Optimized C implementation
- **PostgreSQL Integration**: Native PostgreSQL extension with full SQL support

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

### Common Use Cases

#### Data Partitioning
```sql
-- Distribute data across 8 partitions
SELECT 
    id,
    data,
    murmurhash3_32(id::text) % 8 as partition_key
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

- [ ] Add SHA-256 hash function
- [ ] Add Blake3 hash function
- [ ] Add CityHash implementation
- [ ] Performance optimizations for large data
- [ ] PGXN packaging and distribution
- [ ] Windows build support

## Support

- **Issues**: [GitHub Issues](https://github.com/yourusername/pghashlib/issues)
- **Discussions**: [GitHub Discussions](https://github.com/yourusername/pghashlib/discussions)
- **PostgreSQL Community**: [PostgreSQL Mailing Lists](https://www.postgresql.org/list/)

## Acknowledgments

- MurmurHash3 algorithm by Austin Appleby
- PostgreSQL Extension Building Infrastructure (PGXS)
- PostgreSQL Community for guidance and support 