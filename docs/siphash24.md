# SipHash-2-4

SipHash-2-4 is a cryptographic hash function designed to provide strong protection against hash flooding attacks while maintaining good performance. It uses a 128-bit key for secure hashing.

## Signatures

- `siphash24(text)` → `bigint`
- `siphash24(text, integer, integer)` → `bigint`
- `siphash24(bytea)` → `bigint`
- `siphash24(bytea, integer, integer)` → `bigint`
- `siphash24(integer)` → `bigint`
- `siphash24(integer, integer, integer)` → `bigint`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): First 32-bit seed value for 128-bit key
- Third parameter (optional): Second 32-bit seed value for 128-bit key

## Note

SipHash-2-4 requires a 128-bit key for security. When using custom seeds, provide both 32-bit values. If no seeds are provided, a default key is used.

## Examples

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

## Use Cases

- Protection against hash table denial-of-service attacks
- Secure hash table implementations in web applications
- Message authentication codes (MACs)
- Secure random number generation seeding