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

static inline base62
base62_from_str(const char *str)
{
	int			i = 0,
				d = 0,
				n = strlen(str);
	base62		c = 0;
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

		c += d * base62_powers[n-i-1];

		if (c < 0)
			OUTOFRANGE_ERROR(str, "base62");
	}
	if (neg_sign)
		return 0 - c;

	return c;
}

static inline char *
base62_to_str(base62 c)
{
	int			i,
				d,
				p = 0;
	base62		m = abs(c);
	bool		discard = true;

	char	   *str = palloc0((BASE62_LENGTH + 2) * sizeof(char));

	if (c < 0)
		str[p++] = '-';

	for (i = BASE62_LENGTH - 1; i >= 0; i--)
	{
		d = m / base62_powers[i];
		m = m - base62_powers[i] * d;

		discard = discard && (d == 0 && i > 0);

		if (!discard)
			str[p++] = base62_digits[d];
	}

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
