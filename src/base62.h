#ifndef BASE62_H
#define BASE62_H

#include "postgres.h"

typedef int32 base62;
typedef int64 bigbase62;
typedef int128 hugebase62;

#define BASE62_BASE (62)

static int base62_digits[BASE62_BASE] =
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
	'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
	'u', 'v', 'w', 'x', 'y', 'z'
};

#define OUTOFRANGE_ERROR(_str, _typ)								\
	do {															\
		ereport(ERROR,												\
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),		\
				 errmsg("value \"%s\" is out of range for type %s",	\
						_str, _typ)));								\
	} while(0)

#endif // BASE62_H
