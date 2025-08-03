#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "mb/pg_wchar.h"
#include "access/htup_details.h"

/*
 * lookup3.c, by Bob Jenkins, May 2006, Public Domain.
 * 
 * This is the standard lookup3 implementation that automatically detects
 * endianness and uses the appropriate hash function variant.
 * 
 * These are functions for producing 32-bit hashes for hash table lookup.
 * hashword(), hashlittle(), hashlittle2(), hashbig(), mix(), and final() 
 * are externally useful functions.  Routines to test the hash are included 
 * if SELF_TEST is defined.  You can use this free for any purpose.  It's in
 * the public domain.  It has no warranty.
 *
 * You probably want to use hashlittle().  hashlittle() and hashbig()
 * hash byte arrays.  hashlittle() is is faster than hashbig() on
 * little-endian machines.  Intel and AMD are little-endian machines.
 * On second thought, you probably want hashlittle2(), which is identical to
 * hashlittle() except it returns two 32-bit hashes for the price of one.  
 * You could implement hashbig2() if you wanted but I haven't bothered here.
 *
 * If you want to find a hash of, say, exactly 7 integers, do
 *   a = i1;  b = i2;  c = i3;
 *   mix(a,b,c);
 *   a += i4; b += i5; c += i6;
 *   mix(a,b,c);
 *   a += i7;
 *   final(a,b,c);
 * then use c as the hash value.  If you have a variable length array of
 * 4-byte integers to hash, use hashword().  If you have a byte array (like
 * a character string), use hashlittle().  If you have several byte arrays, or
 * a mix of things, see the comments above hashlittle().  
 *
 * Why is this so big?  I read 12 bytes at a time into 3 4-byte integers, 
 * then mix those integers.  This is fast (you can do a lot more thorough
 * mixing than if you mix one byte at a time).  Second, the results of lots
 * of hash functions are compared in hash.ps.  These hash functions do
 * better than any other hash function I could find, and these hash functions
 * are simple.  Third, these hash functions are designed to prevent malicious
 * users from causing hash flooding.  See hash.doc.
 *
 * See http://burtleburtle.net/bob/c/lookup3.c for the original.
 * Adapted for PostgreSQL extension by adding PostgreSQL function wrappers.
 */

/* Define HASH_LITTLE_ENDIAN based on architecture detection */
#ifndef HASH_LITTLE_ENDIAN
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define HASH_LITTLE_ENDIAN 1
#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define HASH_LITTLE_ENDIAN 0
#elif defined(__LITTLE_ENDIAN__) || defined(__ARMEL__) || defined(__THUMBEL__) || \
      defined(__AARCH64EL__) || defined(_MIPSEL) || defined(__MIPSEL) || \
      defined(__MIPSEL__) || defined(__LITTLE_ENDIAN) || defined(__i386) || \
      defined(__i386__) || defined(__x86_64) || defined(__x86_64__) || \
      defined(_M_IX86) || defined(_M_X64) || defined(_M_AMD64)
#define HASH_LITTLE_ENDIAN 1
#elif defined(__BIG_ENDIAN__) || defined(__ARMEB__) || defined(__THUMBEB__) || \
      defined(__AARCH64EB__) || defined(_MIPSEB) || defined(__MIPSEB) || \
      defined(__MIPSEB__) || defined(__BIG_ENDIAN)
#define HASH_LITTLE_ENDIAN 0
#else
/* Default to little endian if unable to detect */
#define HASH_LITTLE_ENDIAN 1
#endif
#endif

/* Define standard integer types */
#include <stdint.h>

#define hashsize(n) ((uint32_t)1<<(n))
#define hashmask(n) (hashsize(n)-1)
#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

/*
 * mix -- mix 3 32-bit values reversibly.
 */
#define mix(a,b,c) \
{ \
  a -= c;  a ^= rot(c, 4);  c += b; \
  b -= a;  b ^= rot(a, 6);  a += c; \
  c -= b;  c ^= rot(b, 8);  b += a; \
  a -= c;  a ^= rot(c,16);  c += b; \
  b -= a;  b ^= rot(a,19);  a += c; \
  c -= b;  c ^= rot(b, 4);  b += a; \
}

/*
 * final -- final mixing of 3 32-bit values (a,b,c) into c
 */
#define final(a,b,c) \
{ \
  c ^= b; c -= rot(b,14); \
  a ^= c; a -= rot(c,11); \
  b ^= a; b -= rot(a,25); \
  c ^= b; c -= rot(b,16); \
  a ^= c; a -= rot(c,4);  \
  b ^= a; b -= rot(a,14); \
  c ^= b; c -= rot(b,24); \
}



/*
 * hashlittle() -- hash a variable-length key into a 32-bit value
 *   k       : the key (the unaligned variable-length array of bytes)
 *   length  : the length of the key, counting by bytes
 *   initval : can be any 4-byte value
 * Returns a 32-bit value.  Every bit of the key affects every bit of
 * the return value.  Two keys differing by one or two bits will have
 * totally different hash values.
 *
 * The best hash table sizes are powers of 2.  There is no need to do
 * mod a prime (mod is sooo slow!).  If you need less than 32 bits,
 * use a bitmask.  For example, if you need only 10 bits, do
 *   h = (h & hashmask(10));
 * In which case, the hash table should have hashsize(10) elements.
 *
 * If you are hashing n strings (uint8_t **)k, do it like this:
 *   for (i=0, h=0; i<n; ++i) h = hashlittle( k[i], len[i], h);
 *
 * By Bob Jenkins, 2006.  bob_jenkins@burtleburtle.net.  You may use this
 * code any way you wish, private, educational, or commercial.  It's free.
 *
 * Use for hash table lookup, or anything where one collision in 2^^32 is
 * acceptable.  Do NOT use for cryptographic purposes.
 */

static uint32_t hashlittle( const void *key, size_t length, uint32_t initval)
{
  uint32_t a,b,c;                                          /* internal state */
  union { const void *ptr; const uint8_t *c; const uint32_t *k; size_t i; } u;     /* needed for Mac Powerbook G4 */

  /* Set up the internal state */
  a = b = c = 0xdeadbeef + ((uint32_t)length) + initval;

  u.ptr = key;
  if (HASH_LITTLE_ENDIAN && ((u.i & 0x3) == 0)) {
    const uint32_t *k = (const uint32_t *)key;         /* read 32-bit chunks */

    /*------ all but last block: aligned reads and affect 32 bits of (a,b,c) */
    while (length > 12)
    {
      a += k[0];
      b += k[1];
      c += k[2];
      mix(a,b,c);
      length -= 12;
      k += 3;
    }

    /*----------------------------- handle the last (incomplete) block */
    /* 
     * "k[2]&0xffffff" actually reads beyond the end of the string, but
     * then masks off the part it's not allowed to read.  Because the
     * string is aligned, the masked-off tail is in the same word as the
     * rest of the string.  Every machine with memory protection I've seen
     * does it on word boundaries, so is OK with this.  But VALGRIND will
     * still catch it and complain.  The masking trick does make the hash
     * noticeable faster for short strings (like English words).
     */
#ifndef VALGRIND

    switch(length)
    {
    case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
    case 11: c+=k[2]&0xffffff; b+=k[1]; a+=k[0]; break;
    case 10: c+=k[2]&0xffff; b+=k[1]; a+=k[0]; break;
    case 9 : c+=k[2]&0xff; b+=k[1]; a+=k[0]; break;
    case 8 : b+=k[1]; a+=k[0]; break;
    case 7 : b+=k[1]&0xffffff; a+=k[0]; break;
    case 6 : b+=k[1]&0xffff; a+=k[0]; break;
    case 5 : b+=k[1]&0xff; a+=k[0]; break;
    case 4 : a+=k[0]; break;
    case 3 : a+=k[0]&0xffffff; break;
    case 2 : a+=k[0]&0xffff; break;
    case 1 : a+=k[0]&0xff; break;
    case 0 : return c;              /* zero length strings require no mixing */
    }

#else /* make valgrind happy */

    const uint8_t *k8 = (const uint8_t *)k;
    switch(length)
    {
    case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
    case 11: c+=((uint32_t)k8[10])<<16; __attribute__((fallthrough)); /* fall through */
    case 10: c+=((uint32_t)k8[9])<<8; __attribute__((fallthrough)); /* fall through */
    case 9 : c+=k8[8]; __attribute__((fallthrough)); /* fall through */
    case 8 : b+=k[1]; a+=k[0]; break;
    case 7 : b+=((uint32_t)k8[6])<<16; __attribute__((fallthrough)); /* fall through */
    case 6 : b+=((uint32_t)k8[5])<<8; __attribute__((fallthrough)); /* fall through */
    case 5 : b+=k8[4]; __attribute__((fallthrough)); /* fall through */
    case 4 : a+=k[0]; break;
    case 3 : a+=((uint32_t)k8[2])<<16; __attribute__((fallthrough)); /* fall through */
    case 2 : a+=((uint32_t)k8[1])<<8; __attribute__((fallthrough)); /* fall through */
    case 1 : a+=k8[0]; break;
    case 0 : return c;
    }

#endif /* !valgrind */

  } else if (HASH_LITTLE_ENDIAN && ((u.i & 0x1) == 0)) {
    const uint16_t *k = (const uint16_t *)key;           /* read 16-bit chunks */
    const uint8_t  *k8;

    /*--------------- all but last block: aligned reads and different mixing */
    while (length > 12)
    {
      a += k[0] + (((uint32_t)k[1])<<16);
      b += k[2] + (((uint32_t)k[3])<<16);
      c += k[4] + (((uint32_t)k[5])<<16);
      mix(a,b,c);
      length -= 12;
      k += 6;
    }

    /*----------------------------- handle the last (incomplete) block */
    k8 = (const uint8_t *)k;
    switch(length)
    {
    case 12: c+=k[4]+(((uint32_t)k[5])<<16); b+=k[2]+(((uint32_t)k[3])<<16); a+=k[0]+(((uint32_t)k[1])<<16); break;
    case 11: c+=((uint32_t)k8[10])<<16; __attribute__((fallthrough)); /* fall through */
    case 10: c+=k[4]; b+=k[2]+(((uint32_t)k[3])<<16); a+=k[0]+(((uint32_t)k[1])<<16); break;
    case 9 : c+=k8[8]; __attribute__((fallthrough)); /* fall through */
    case 8 : b+=k[2]+(((uint32_t)k[3])<<16); a+=k[0]+(((uint32_t)k[1])<<16); break;
    case 7 : b+=((uint32_t)k8[6])<<16; __attribute__((fallthrough)); /* fall through */
    case 6 : b+=k[2]; a+=k[0]+(((uint32_t)k[1])<<16); break;
    case 5 : b+=k8[4]; __attribute__((fallthrough)); /* fall through */
    case 4 : a+=k[0]+(((uint32_t)k[1])<<16); break;
    case 3 : a+=((uint32_t)k8[2])<<16; __attribute__((fallthrough)); /* fall through */
    case 2 : a+=k[0]; break;
    case 1 : a+=k8[0]; break;
    case 0 : return c;                     /* zero length requires no mixing */
    }

  } else {                        /* need to read the key one byte at a time */
    const uint8_t *k = (const uint8_t *)key;

    /*--------------- all but the last block: affect some 32 bits of (a,b,c) */
    while (length > 12)
    {
      a += k[0];
      a += ((uint32_t)k[1])<<8;
      a += ((uint32_t)k[2])<<16;
      a += ((uint32_t)k[3])<<24;
      b += k[4];
      b += ((uint32_t)k[5])<<8;
      b += ((uint32_t)k[6])<<16;
      b += ((uint32_t)k[7])<<24;
      c += k[8];
      c += ((uint32_t)k[9])<<8;
      c += ((uint32_t)k[10])<<16;
      c += ((uint32_t)k[11])<<24;
      mix(a,b,c);
      length -= 12;
      k += 12;
    }

    /*-------------------------------- last block: affect all 32 bits of (c) */
    switch(length)                   /* all the case statements fall through */
    {
    case 12: c+=((uint32_t)k[11])<<24; __attribute__((fallthrough));
    case 11: c+=((uint32_t)k[10])<<16; __attribute__((fallthrough));
    case 10: c+=((uint32_t)k[9])<<8; __attribute__((fallthrough));
    case 9 : c+=k[8]; __attribute__((fallthrough));
    case 8 : b+=((uint32_t)k[7])<<24; __attribute__((fallthrough));
    case 7 : b+=((uint32_t)k[6])<<16; __attribute__((fallthrough));
    case 6 : b+=((uint32_t)k[5])<<8; __attribute__((fallthrough));
    case 5 : b+=k[4]; __attribute__((fallthrough));
    case 4 : a+=((uint32_t)k[3])<<24; __attribute__((fallthrough));
    case 3 : a+=((uint32_t)k[2])<<16; __attribute__((fallthrough));
    case 2 : a+=((uint32_t)k[1])<<8; __attribute__((fallthrough));
    case 1 : a+=k[0];
         break;
    case 0 : return c;
    }
  }

  final(a,b,c);
  return c;
}

/*
 * Standard lookup3 hash function - always uses little-endian variant
 * for consistent results across platforms
 */
static uint32_t lookup3_hash(const void *key, size_t length, uint32_t initval)
{
    return hashlittle(key, length, initval);
}

/* PostgreSQL function declarations */
PG_FUNCTION_INFO_V1(lookup3_text);
PG_FUNCTION_INFO_V1(lookup3_text_seed);
PG_FUNCTION_INFO_V1(lookup3_bytea);
PG_FUNCTION_INFO_V1(lookup3_bytea_seed);
PG_FUNCTION_INFO_V1(lookup3_int);
PG_FUNCTION_INFO_V1(lookup3_int_seed);

/*
 * lookup3 hash function for text input (default initval = 0)
 */
Datum
lookup3_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *str;
    int len;
    uint32_t hash;

    str = VARDATA_ANY(input);
    len = VARSIZE_ANY_EXHDR(input);

    hash = lookup3_hash(str, len, 0);

    PG_RETURN_INT32((int32_t)hash);
}

/*
 * lookup3 hash function for text input with custom initval
 */
Datum
lookup3_text_seed(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int32 seed = PG_GETARG_INT32(1);
    char *str;
    int len;
    uint32_t hash;

    str = VARDATA_ANY(input);
    len = VARSIZE_ANY_EXHDR(input);

    hash = lookup3_hash(str, len, (uint32_t)seed);

    PG_RETURN_INT32((int32_t)hash);
}

/*
 * lookup3 hash function for bytea input (default initval = 0)
 */
Datum
lookup3_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    unsigned char *data;
    int len;
    uint32_t hash;

    data = (unsigned char *) VARDATA_ANY(input);
    len = VARSIZE_ANY_EXHDR(input);

    hash = lookup3_hash(data, len, 0);

    PG_RETURN_INT32((int32_t)hash);
}

/*
 * lookup3 hash function for bytea input with custom initval
 */
Datum
lookup3_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int32 seed = PG_GETARG_INT32(1);
    unsigned char *data;
    int len;
    uint32_t hash;

    data = (unsigned char *) VARDATA_ANY(input);
    len = VARSIZE_ANY_EXHDR(input);

    hash = lookup3_hash(data, len, (uint32_t)seed);

    PG_RETURN_INT32((int32_t)hash);
}

/*
 * lookup3 hash function for integer input (default initval = 0)
 */
Datum
lookup3_int(PG_FUNCTION_ARGS)
{
    int32 input = PG_GETARG_INT32(0);
    uint32_t hash;

    hash = lookup3_hash(&input, sizeof(int32), 0);

    PG_RETURN_INT32((int32_t)hash);
}

/*
 * lookup3 hash function for integer input with custom initval
 */
Datum
lookup3_int_seed(PG_FUNCTION_ARGS)
{
    int32 input = PG_GETARG_INT32(0);
    int32 seed = PG_GETARG_INT32(1);
    uint32_t hash;

    hash = lookup3_hash(&input, sizeof(int32), (uint32_t)seed);

    PG_RETURN_INT32((int32_t)hash);
}