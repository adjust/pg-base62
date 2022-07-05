#include "base62.h"
#include "fmgr.h"

#include "utils/builtins.h"

PG_MODULE_MAGIC;

#define BASE62_LENGTH	6

static base62 base62_powers[BASE62_LENGTH] =
{
	1,
	62,
	3844,
	238328,
	14776336,
	916132832
};

PG_FUNCTION_INFO_V1(base62_in);
PG_FUNCTION_INFO_V1(base62_out);
PG_FUNCTION_INFO_V1(base62_cast_from_text);
PG_FUNCTION_INFO_V1(base62_cast_to_text);

/*
 * mul_s32_overflow and add_s32_overflow are copied from Postgres source codes
 * because not all versions have it.
 */
static inline bool
mul_s32_overflow(int32 a, int32 b, int32 *result)
{
#if defined(HAVE__BUILTIN_OP_OVERFLOW)
	return __builtin_mul_overflow(a, b, result);
#else
	int64		res = (int64) a * (int64) b;

	if (res > PG_INT32_MAX || res < PG_INT32_MIN)
	{
		*result = 0x5EED;		/* to avoid spurious warnings */
		return true;
	}
	*result = (int32) res;
	return false;
#endif
}

static inline bool
add_s32_overflow(int32 a, int32 b, int32 *result)
{
#if defined(HAVE__BUILTIN_OP_OVERFLOW)
	return __builtin_add_overflow(a, b, result);
#else
	int64		res = (int64) a + (int64) b;

	if (res > PG_INT32_MAX || res < PG_INT32_MIN)
	{
		*result = 0x5EED;		/* to avoid spurious warnings */
		return true;
	}
	*result = (int32) res;
	return false;
#endif
}

static inline base62
base62_from_str(const char *str)
{
	int			i = 0,
				n = strlen(str);
	base62		res = 0;
	bool		neg_sign = false;

	if (n == 0)
	{
		OUTOFRANGE_ERROR(str, "base62");
	}
	else if(str[0] == '-')
	{
		if (n - 1 > BASE62_LENGTH)
			OUTOFRANGE_ERROR(str, "base62");
		neg_sign = true;
		i = 1;
	}
	else if (n > BASE62_LENGTH)
	{
		OUTOFRANGE_ERROR(str, "base62");
	}

	for (; i < n; i++)
	{
		int32		d;
		int32		res_buf;

		if (str[i] >= '0' && str[i] <= '9')
			d = str[i] - '0';
		else if (str[i] >= 'A' && str[i] <= 'Z')
			d = 10 + str[i] - 'A';
		else if (str[i] >= 'a' && str[i] <= 'z')
			d = 36 + str[i] - 'a';
		else
			ereport(ERROR,
					(errcode(ERRCODE_SYNTAX_ERROR),
					 errmsg("value \"%c\" is not a valid digit for type base62", str[i])));

		if (mul_s32_overflow(d, base62_powers[n - i - 1], &res_buf) ||
			add_s32_overflow(res, res_buf, &res))
			OUTOFRANGE_ERROR(str, "base62");
	}
	if (neg_sign)
		return 0 - res;

	return res;
}

static inline char *
base62_to_str(base62 c)
{
	int			i,
				p = 0;
	base62		m = abs(c);
	bool		discard = true;

	char	   *str = palloc((BASE62_LENGTH + 2) * sizeof(char));

	if (c < 0)
		str[p++] = '-';

	for (i = BASE62_LENGTH - 1; i >= 0; i--)
	{
		int32		d;

		d = m / base62_powers[i];
		m = m - base62_powers[i] * d;

		discard = discard && (d == 0 && i > 0);

		if (!discard)
			str[p++] = base62_digits[d];
	}

	// Null terminator
	str[p] = '\0';

	return str;
}

Datum
base62_in(PG_FUNCTION_ARGS)
{
	char	   *str = PG_GETARG_CSTRING(0);

	PG_RETURN_INT32(base62_from_str(str));
}

Datum
base62_out(PG_FUNCTION_ARGS)
{
	base62		c = PG_GETARG_INT32(0);

	PG_RETURN_CSTRING(base62_to_str(c));
}

Datum
base62_cast_from_text(PG_FUNCTION_ARGS)
{
	text	   *txt = PG_GETARG_TEXT_P(0);

	PG_RETURN_INT32(base62_from_str(text_to_cstring(txt)));
}

Datum
base62_cast_to_text(PG_FUNCTION_ARGS)
{
	base62		c  = PG_GETARG_INT32(0);

	PG_RETURN_TEXT_P(cstring_to_text(base62_to_str(c)));
}
