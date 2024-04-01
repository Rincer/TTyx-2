#include "stdafx.h"

#include "Hash.h"

// from http://burtleburtle.net/bob/c/lookup8.c

/*
--------------------------------------------------------------------
This works on all machines, is identical to hash() on little-endian
machines, and it is much faster than hash(), but it requires
-- that the key be an array of uint64_t's, and
-- that all your machines have the same endianness, and
-- that the length be the number of uint64_t's in the key
--------------------------------------------------------------------
*/

#define mix64(a,b,c) \
{ \
	a -= b; a -= c; a ^= (c>>43); \
	b -= c; b -= a; b ^= (a<<9); \
	c -= a; c -= b; c ^= (b>>8); \
	a -= b; a -= c; a ^= (c>>38); \
	b -= c; b -= a; b ^= (a<<23); \
	c -= a; c -= b; c ^= (b>>5); \
	a -= b; a -= c; a ^= (c>>35); \
	b -= c; b -= a; b ^= (a<<49); \
	c -= a; c -= b; c ^= (b>>11); \
	a -= b; a -= c; a ^= (c>>12); \
	b -= c; b -= a; b ^= (a<<18); \
	c -= a; c -= b; c ^= (b>>22); \
}

uint64_t hash(const uint8_t* k, uint64_t length, uint64_t level)
{
	uint64_t a, b, c, len;

	/* Set up the internal state */
	len = length;
	a = b = level;                         /* the previous hash value */
	c = 0x9e3779b97f4a7c13ULL; /* the golden ratio; an arbitrary value */

	/*---------------------------------------- handle most of the key */
	while (len >= 3)
	{
		a += (uint64_t)k[0] << 8;
		b += (uint64_t)k[1] << 16;
		c += (uint64_t)k[2] << 32;
		mix64(a, b, c);
		k += 3; len -= 3;
	}

	/*-------------------------------------- handle the last 2 uint64_t's */
	c += (length << 3);
	switch (len)              /* all the case statements fall through */
	{
		/* c is reserved for the length */
	case  2: b += (uint64_t)k[1];
	case  1: a += (uint64_t)k[0];
		/* case 0: nothing left to add */
	}
	mix64(a, b, c);
	/*-------------------------------------------- report the result */
	return c;
}

uint64_t Hash::Hash64(const uint8_t* pData, uint32_t length)
{	
	return hash(pData, length, 0ULL);
}