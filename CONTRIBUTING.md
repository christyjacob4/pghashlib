# Contributing to PostgreSQL Hash Library (hashlib)

Thank you for your interest in contributing to pghashlib! This document provides guidelines and information for contributors.

## Getting Started

1. Fork the repository
2. Create a feature branch
3. Add tests for new functionality
4. Ensure all tests pass
5. Submit a pull request

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

### Code Style

- Follow C90 standard
- Use PostgreSQL coding conventions
- All variable declarations must be at the beginning of functions
- No mixed declarations and code
- Handle all compiler warnings (treat warnings as errors)

### Testing Guidelines

- Add comprehensive test cases for new functions
- Test with different input types (text, bytea, integer)
- Test both with and without seed parameters
- Verify consistent results across multiple runs
- Include edge cases (empty strings, large inputs, etc.)

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
See installation instructions in README.md.

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

### Creating Releases

1. **Update version numbers** in `META.json` and `hashlib.control`
2. **Update changelog** with new features and fixes
3. **Run full test suite** across all supported PostgreSQL versions
4. **Create git tag** with version number
5. **Generate release archive**:
   ```bash
   make dist
   ```
6. **Upload to GitHub releases** with checksums
7. **Submit to PGXN** (if applicable)

### PGXN Packaging

To package for PGXN:

1. **Validate META.json**:
   ```bash
   make validate-meta
   ```

2. **Create distribution**:
   ```bash
   make dist
   ```

3. **Test the package**:
   ```bash
   pgxn install ./hashlib-0.1.0.zip
   ```

4. **Submit to PGXN**:
   ```bash
   pgxn release ./hashlib-0.1.0.zip
   ```

## Performance Considerations

When contributing new hash functions:

- **Benchmark against existing functions** using representative data
- **Profile memory usage** to ensure minimal overhead
- **Test with various input sizes** (small strings, large blobs, integers)
- **Consider SIMD optimizations** for performance-critical paths
- **Document performance characteristics** in function documentation

## Documentation Standards

- **Update README.md** with new function documentation
- **Include usage examples** with expected outputs
- **Document performance characteristics**
- **Add to compatibility matrix** if function has specific requirements
- **Update function table** with new entries

## Compatibility Requirements

- **C90 compliance** - code must compile with strict C90 standards
- **PostgreSQL 12+** - maintain compatibility with PostgreSQL 12 through latest
- **Cross-platform** - test on Linux, macOS, and Windows (via WSL)
- **Multiple architectures** - ensure compatibility with x86_64 and ARM64

## Security Considerations

- **No cryptographic claims** - clearly document that hash functions are non-cryptographic
- **Input validation** - handle edge cases gracefully without crashes
- **Memory safety** - prevent buffer overflows and memory leaks
- **Seed handling** - ensure seeds don't expose sensitive information

## Community Guidelines

- **Be respectful** and inclusive in all communications
- **Follow code of conduct** (if applicable)
- **Help newcomers** and answer questions
- **Review pull requests** constructively
- **Report issues** with clear reproduction steps

## Getting Help

- **GitHub Issues**: [Report bugs and request features](https://github.com/yourusername/pghashlib/issues)
- **GitHub Discussions**: [Ask questions and discuss ideas](https://github.com/yourusername/pghashlib/discussions)
- **PostgreSQL Community**: [PostgreSQL Mailing Lists](https://www.postgresql.org/list/)

## Development Setup

### Using Docker (Recommended)

```bash
# Start development environment
docker compose up -d

# Run tests
docker compose exec postgres make test

# Interactive development
docker compose exec postgres bash
```

### Local Development

```bash
# Install PostgreSQL development headers
sudo apt-get install postgresql-server-dev-all

# Build and test
make clean
make
make installcheck
```

### IDE Setup

For better development experience:

- **VS Code**: Install C/C++ extension and PostgreSQL extension
- **CLion**: Configure with PostgreSQL headers path
- **Vim/Neovim**: Use Language Server Protocol (LSP) with clangd

## Troubleshooting

### Common Build Issues

- **Missing headers**: Install `postgresql-server-dev-all` package
- **Wrong PostgreSQL version**: Use `PG_CONFIG` to specify correct version
- **Permission denied**: Use `sudo` for installation commands

### Common Test Failures

- **Extension not loaded**: Ensure `CREATE EXTENSION hashlib;` succeeds
- **Function not found**: Check SQL function definitions in `sql/` directory
- **Unexpected results**: Verify test expectations match actual output

Thank you for contributing to pghashlib!