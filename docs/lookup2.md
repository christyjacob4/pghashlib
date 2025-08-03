# lookup2

lookup2 is Bob Jenkins' hash function, designed for fast hashing with good distribution properties.

## Signatures

- `lookup2(text)` → `integer`
- `lookup2(text, integer)` → `integer`
- `lookup2(bytea)` → `integer`
- `lookup2(bytea, integer)` → `integer`
- `lookup2(integer)` → `integer`
- `lookup2(integer, integer)` → `integer`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Initial value/seed (default: 0)

## Examples

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

## Use Cases

- Classic hash table implementations
- Legacy system compatibility
- Simple, reliable hashing with proven track record
- Educational purposes studying hash function evolution