# lookup3be

lookup3be is Bob Jenkins' lookup3 hash function with big-endian byte order, an improved version of lookup2 with better mixing and avalanche properties.

## Signatures

- `lookup3be(text)` → `integer`
- `lookup3be(text, integer)` → `integer`
- `lookup3be(bytea)` → `integer`
- `lookup3be(bytea, integer)` → `integer`
- `lookup3be(integer)` → `integer`
- `lookup3be(integer, integer)` → `integer`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Initial value/seed (default: 0)

## Examples

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

## Use Cases

- Big-endian system compatibility
- Improved hash table implementations over lookup2
- Network protocols requiring big-endian byte order
- Cross-platform consistency on big-endian architectures