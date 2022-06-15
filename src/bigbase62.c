#include "base62.h"
#include "fmgr.h"

#include "utils/builtins.h"

#define BIGBASE62_LENGTH	11

static bigbase62 bigbase62_powers[BIGBASE62_LENGTH] =
{
	1ULL,
	62ULL,
	3844ULL,
	238328ULL,
	14776336ULL,
	916132832ULL,
	56800235584ULL,
	3521614606208ULL,
	218340105584896ULL,
	13537086546263552ULL,
	839299365868340224ULL
};

PG_FUNCTION_INFO_V1(bigbase62_in);
PG_FUNCTION_INFO_V1(bigbase62_out);
PG_FUNCTION_INFO_V1(bigbase62_cast_from_text);
PG_FUNCTION_INFO_V1(bigbase62_cast_to_text);

static inline bigbase62
bigbase62_from_str(const char *str)
{
	int			i = 0,
				d = 0,
				n = strlen(str);
	bigbase62	c = 0;
	bool		neg_sign = false;

	if (n == 0){
		OUTOFRANGE_ERROR(str, "bigbase62");
	}
	else if(str[0] == '-')
	{
		if (n - 1 > BIGBASE62_LENGTH)
			OUTOFRANGE_ERROR(str, "bigbase62");

		neg_sign = true;
		i = 1;
	}
	else if (n > BIGBASE62_LENGTH)
	{
		OUTOFRANGE_ERROR(str, "bigbase62");
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
					 errmsg("value \"%c\" is not a valid digit for type bigbase62", str[i])));

		c += d * bigbase62_powers[n - i - 1];

		if (c < 0)
			OUTOFRANGE_ERROR(str, "bigbase62");
	}
	if (neg_sign)
		return 0 - c;

	return c;
}

static inline char *
bigbase62_to_str(bigbase62 c)
{
	int			i,
				d,
				p = 0;
	bigbase62	m = labs(c);
	bool		discard = true;
	char	   *str = palloc0((BIGBASE62_LENGTH + 2) * sizeof(char));

	if (c < 0)
		str[p++] = '-';

	for (i = BIGBASE62_LENGTH - 1; i >= 0; i--)
	{
		d = m / bigbase62_powers[i];
		m = m - bigbase62_powers[i] * d;

		discard = discard && (d == 0 && i > 0);

		if (!discard)
			str[p++] = base62_digits[d];
	}

	return str;
}

Datum
bigbase62_in(PG_FUNCTION_ARGS)
{
	char	   *str = PG_GETARG_CSTRING(0);

	PG_RETURN_INT64(bigbase62_from_str(str));
}

Datum
bigbase62_out(PG_FUNCTION_ARGS)
{
	bigbase62	c = PG_GETARG_INT64(0);

	PG_RETURN_CSTRING(bigbase62_to_str(c));
}

Datum
bigbase62_cast_from_text(PG_FUNCTION_ARGS)
{
	text	   *txt = PG_GETARG_TEXT_P(0);

	PG_RETURN_INT64(bigbase62_from_str(text_to_cstring(txt)));
}

Datum
bigbase62_cast_to_text(PG_FUNCTION_ARGS)
{
	bigbase62	c  = PG_GETARG_INT64(0);

	PG_RETURN_TEXT_P(cstring_to_text(bigbase62_to_str(c)));
}
