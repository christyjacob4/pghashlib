# Hash Algorithms Documentation

This document provides an index to all hash algorithms implemented in pghashlib. Each algorithm has its own detailed documentation file.

## Available Hash Functions

### Fast Non-Cryptographic Hashes
- **[MurmurHash3](murmurhash3.md)** - Fast, general-purpose hash function
- **[xxHash32](xxhash32.md)** - Extremely fast 32-bit hash 
- **[xxHash64](xxhash64.md)** - Extremely fast 64-bit hash
- **[xxHash3_64](xxhash3_64.md)** - Next-generation 64-bit xxHash with improved performance and quality
- **[WyHash](wyhash.md)** - One of the fastest quality hash functions (used by Go, Zig, V, Nim)
- **[t1ha0](t1ha0.md)** - Fastest t1ha variant, CPU-optimized
- **[t1ha1](t1ha1.md)** - Portable t1ha variant
- **[t1ha2](t1ha2.md)** - Recommended t1ha variant with best speed/quality balance

### High-Quality Distribution Hashes
- **[CityHash64](cityhash64.md)** - Google's high-performance 64-bit hash
- **[CityHash128](cityhash128.md)** - Google's high-performance 128-bit hash
- **[FarmHash32](farmhash32.md)** - Google's successor to CityHash (32-bit)
- **[FarmHash64](farmhash64.md)** - Google's successor to CityHash (64-bit)
- **[MetroHash64](metrohash64.md)** - Fast alternative with excellent avalanche properties
- **[MetroHash128](metrohash128.md)** - 128-bit version with MD5-like statistical profile

### Extended Length Hashes
- **[CityHash128](cityhash128.md)** - 128-bit output for strong collision resistance
- **[xxHash3_128](xxhash3_128.md)** - Next-generation 128-bit xxHash with superior quality and performance
- **[SpookyHash128](spookyhash128.md)** - Bob Jenkins' 128-bit hash
- **[MetroHash128](metrohash128.md)** - 128-bit output with excellent properties
- **[t1ha2_128](t1ha2_128.md)** - 128-bit version of t1ha2
- **[HighwayHash128](highwayhash128.md)** - SIMD-optimized 128-bit keyed hash
- **[HighwayHash256](highwayhash256.md)** - SIMD-optimized 256-bit keyed hash

### Security-Focused Hashes
- **[SipHash-2-4](siphash24.md)** - Cryptographic hash for protection against hash flooding attacks
- **[HighwayHash64](highwayhash64.md)** - SIMD-optimized keyed hash (64-bit)
- **[HighwayHash128](highwayhash128.md)** - SIMD-optimized keyed hash (128-bit)
- **[HighwayHash256](highwayhash256.md)** - SIMD-optimized keyed hash (256-bit)

### Classic & Legacy Hashes
- **[CRC32](crc32.md)** - Cyclic redundancy check for error detection
- **[SpookyHash64](spookyhash64.md)** - Bob Jenkins' 64-bit hash optimized for 64-bit processors
- **[lookup2](lookup2.md)** - Bob Jenkins' classic hash function
- **[lookup3be](lookup3be.md)** - Bob Jenkins' lookup3 with big-endian byte order
- **[lookup3le](lookup3le.md)** - Bob Jenkins' lookup3 with little-endian byte order

## Performance Guide

### Fastest Performance
- **WyHash** - Extremely fast with excellent quality
- **xxHash3_64** - Next-generation ultra-fast 64-bit hashing
- **t1ha0** - CPU-optimized, fastest t1ha variant
- **xxHash64** - Ultra-fast 64-bit hashing
- **xxHash32** - Ultra-fast 32-bit hashing

### Best Distribution Quality
- **CityHash64/128** - Google's high-quality distribution
- **FarmHash32/64** - Google's improved successor to CityHash
- **MetroHash64/128** - Excellent avalanche properties

### Security Requirements
- **SipHash-2-4** - Cryptographic protection against attacks
- **HighwayHash** (64/128/256) - SIMD-optimized with cryptographic security

### Extended Hash Length
- **xxHash3_128** - Fast 128-bit with superior quality and performance
- **CityHash128** - Strong 128-bit collision resistance
- **HighwayHash256** - Maximum 256-bit collision resistance
- **t1ha2_128** - Fast 128-bit with good quality

## Common Use Cases

### Data Partitioning
- **Recommended**: CityHash64, FarmHash64, WyHash
- **For many partitions**: CityHash128, MetroHash128

### Random Sampling
- **Recommended**: MurmurHash3, WyHash, xxHash64

### Security-Sensitive Applications
- **Recommended**: SipHash-2-4, HighwayHash family

### Cross-Platform Consistency
- **Recommended**: t1ha1, lookup3le, CityHash family

### Maximum Performance
- **Recommended**: WyHash, xxHash3_64, t1ha0, xxHash64

Each algorithm documentation includes detailed function signatures, parameters, examples, and specific use case recommendations.