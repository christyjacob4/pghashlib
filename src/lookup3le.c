#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "mb/pg_wchar.h"
#include "access/htup_details.h"

/*
 * lookup3le.c, by Bob Jenkins, May 2006, Public Domain.
 * 
 * This is lookup3.c, but modified for little-endian byte order.
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
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define HASH_LITTLE_ENDIAN 1
#elif defined(__LITTLE_ENDIAN__) || defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || \
      defined(_LITTLE_ENDIAN) || defined(__i386) || defined(__i386__) || defined(_M_IX86) || \
      defined(__x86_64) || defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64)
#define HASH_LITTLE_ENDIAN 1
#else
#define HASH_LITTLE_ENDIAN 0
#endif
#endif

#define hashsize(n) ((uint32_t)1<<(n))
#define hashmask(n) (hashsize(n)-1)
#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

/*
 * mix -- mix 3 32-bit values reversibly.
 *
 * This is reversible, so any information in (a,b,c) before mix() is
 * still in (a,b,c) after mix().
 *
 * If four pairs of (a,b,c) inputs are run through mix(), or through
 * mix() in reverse, there are at least 32 bits of the output that
 * are sometimes the same for one pair and different for another pair.
 * This was tested for:
 * * pairs that differed by one bit, by two bits, in any combination
 *   of top bits of (a,b,c), or in any combination of bottom bits of
 *   (a,b,c).
 * * "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
 *   the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
 *   is commonly produced by subtraction) look like a single 1-bit
 *   difference.
 * * the base values were pseudorandom, all zero but one bit set, or 
 *   all zero plus a counter, or all zero plus a counter plus a small
 *   constant.
 *
 * Some k values for my "a-=c; a^=rot(c,k); c+=b;" arrangement that
 * satisfy this are
 *     4  6  8 16 19  4
 *     9 15  3 18 27 15
 *    14  9  3  7 17  3
 * Well, "9 15 3 18 27 15" didn't quite get 32 bits diffing
 * for "differ" defined as + with a one-bit base and a two-bit delta.  I
 * used http://burtleburtle.net/bob/hash/avalanche.html to choose 
 * the operations, constants, and arrangements of the variables.
 *
 * This does not achieve avalanche.  There are input bits of (a,b,c)
 * that fail to affect some output bits of (a,b,c), especially of a.  The
 * most thoroughly mixed value is c, but it doesn't really even achieve
 * avalanche in c.
 *
 * This allows some parallelism.  Read-after-writes are good at doubling
 * the number of bits affected, so the goal of mixing pulls in the opposite
 * direction as the goal of parallelism.  I did what I could.  Rotates
 * seem to cost as much as shifts on every machine I could lay my hands
 * on, and rotates are much kinder to the top and bottom bits, so I used
 * rotates.
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
 *
 * Pairs of (a,b,c) values differing in only a few bits will usually
 * produce values of c that look totally different.  This was tested for
 * * pairs that differed by one bit, by two bits, in any combination
 *   of top bits of (a,b,c), or in any combination of bottom bits of
 *   (a,b,c).
 * * "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
 *   the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
 *   is commonly produced by subtraction) look like a single 1-bit
 *   difference.
 * * the base values were pseudorandom, all zero but one bit set, or 
 *   all zero plus a counter, or all zero plus a counter plus a small
 *   constant.
 *
 * These constants passed:
 *  14 11 25 16 4 14 24
 *  12 14 25 16 4 14 24
 * and these came close:
 *   4  8 15 26 3 22 24
 *  10  8 15 26 3 22 24
 *  11  8 15 26 3 22 24
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
 * hashlittle():
 * This is the same as hashword() on little-endian machines.  It is different
 * from hashbig() on all machines.  hashlittle() takes advantage of
 * little-endian byte ordering. 
 */
static uint32_t hashlittle( const void *key, size_t length, uint32_t initval)
{
  uint32_t a,b,c;                                          /* internal state */
  union { const void *ptr; size_t i; } u;     /* needed for Mac Powerbook G4 */

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
    const uint16_t *k = (const uint16_t *)key;         /* read 16-bit chunks */
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
    case 12: c+=k[4]+(((uint32_t)k[5])<<16);
             b+=k[2]+(((uint32_t)k[3])<<16);
             a+=k[0]+(((uint32_t)k[1])<<16);
             break;
    case 11: c+=((uint32_t)k8[10])<<16; __attribute__((fallthrough)); /* fall through */
    case 10: c+=k[4];
             b+=k[2]+(((uint32_t)k[3])<<16);
             a+=k[0]+(((uint32_t)k[1])<<16);
             break;
    case 9 : c+=k8[8]; __attribute__((fallthrough)); /* fall through */
    case 8 : b+=k[2]+(((uint32_t)k[3])<<16);
             a+=k[0]+(((uint32_t)k[1])<<16);
             break;
    case 7 : b+=((uint32_t)k8[6])<<16; __attribute__((fallthrough)); /* fall through */
    case 6 : b+=k[2];
             a+=k[0]+(((uint32_t)k[1])<<16);
             break;
    case 5 : b+=k8[4]; __attribute__((fallthrough)); /* fall through */
    case 4 : a+=k[0]+(((uint32_t)k[1])<<16);
             break;
    case 3 : a+=((uint32_t)k8[2])<<16; __attribute__((fallthrough)); /* fall through */
    case 2 : a+=k[0];
             break;
    case 1 : a+=k8[0];
             break;
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

/* lookup3le for text input with default initval */
PG_FUNCTION_INFO_V1(lookup3le_text);

Datum
lookup3le_text(PG_FUNCTION_ARGS)
{
    text *input;
    char *data;
    int len;
    uint32_t hash_result;
    
    input = PG_GETARG_TEXT_PP(0);
    data = VARDATA_ANY(input);
    len = VARSIZE_ANY_EXHDR(input);
    hash_result = hashlittle(data, len, 0);
    
    PG_RETURN_INT32((int32_t)hash_result);
}

/* lookup3le for text input with custom initval */
PG_FUNCTION_INFO_V1(lookup3le_text_seed);

Datum
lookup3le_text_seed(PG_FUNCTION_ARGS)
{
    text *input;
    int32_t initval;
    char *data;
    int len;
    uint32_t hash_result;
    
    input = PG_GETARG_TEXT_PP(0);
    initval = PG_GETARG_INT32(1);
    data = VARDATA_ANY(input);
    len = VARSIZE_ANY_EXHDR(input);
    hash_result = hashlittle(data, len, (uint32_t)initval);
    
    PG_RETURN_INT32((int32_t)hash_result);
}

/* lookup3le for bytea input with default initval */
PG_FUNCTION_INFO_V1(lookup3le_bytea);

Datum
lookup3le_bytea(PG_FUNCTION_ARGS)
{
    bytea *input;
    char *data;
    int len;
    uint32_t hash_result;
    
    input = PG_GETARG_BYTEA_PP(0);
    data = VARDATA_ANY(input);
    len = VARSIZE_ANY_EXHDR(input);
    hash_result = hashlittle(data, len, 0);
    
    PG_RETURN_INT32((int32_t)hash_result);
}

/* lookup3le for bytea input with custom initval */
PG_FUNCTION_INFO_V1(lookup3le_bytea_seed);

Datum
lookup3le_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input;
    int32_t initval;
    char *data;
    int len;
    uint32_t hash_result;
    
    input = PG_GETARG_BYTEA_PP(0);
    initval = PG_GETARG_INT32(1);
    data = VARDATA_ANY(input);
    len = VARSIZE_ANY_EXHDR(input);
    hash_result = hashlittle(data, len, (uint32_t)initval);
    
    PG_RETURN_INT32((int32_t)hash_result);
}

/* lookup3le for integer input with default initval */
PG_FUNCTION_INFO_V1(lookup3le_int);

Datum
lookup3le_int(PG_FUNCTION_ARGS)
{
    int32_t input;
    uint32_t hash_result;
    
    input = PG_GETARG_INT32(0);
    hash_result = hashlittle(&input, sizeof(int32_t), 0);
    
    PG_RETURN_INT32((int32_t)hash_result);
}

/* lookup3le for integer input with custom initval */
PG_FUNCTION_INFO_V1(lookup3le_int_seed);

Datum
lookup3le_int_seed(PG_FUNCTION_ARGS)
{
    int32_t input, initval;
    uint32_t hash_result;
    
    input = PG_GETARG_INT32(0);
    initval = PG_GETARG_INT32(1);
    hash_result = hashlittle(&input, sizeof(int32_t), (uint32_t)initval);
    
    PG_RETURN_INT32((int32_t)hash_result);
}