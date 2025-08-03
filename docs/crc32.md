# CRC32

CRC32 is a cyclic redundancy check algorithm commonly used for error detection.

## Signatures

- `crc32(text)` → `integer`
- `crc32(text, integer)` → `integer`
- `crc32(bytea)` → `integer`
- `crc32(bytea, integer)` → `integer`
- `crc32(integer)` → `integer`
- `crc32(integer, integer)` → `integer`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

## Examples

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

## Use Cases

- Error detection and data integrity verification
- Checksum validation for data transmission
- Simple data fingerprinting