#include "base62.h"
#include "fmgr.h"

#include "utils/builtins.h"

#define HUGEBASE62_NONZERO_POWERS	11
#define HUGEBASE62_LENGTH			22

static hugebase62 hugebase62_powers[HUGEBASE62_LENGTH] =
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
	839299365868340224ULL,
	0ULL,
	0ULL,
	0ULL,
	0ULL,
	0ULL,
	0ULL,
	0ULL,
	0ULL,
	0ULL,
	0ULL,
	0ULL

	// We cannot define literals for following values, because they are too large
	// 52036560683837093888
	// 3226266762397899821056
	// 200028539268669788905472
	// 12401769434657526912139264
	// 768909704948766668552634368
	// 47672401706823533450263330816
	// 2955688905823059073916326510592
	// 183252712161029662582812243656704
	// 11361668153983839080134359106715648
	// 704423425546998022968330264616370176
	// 43674252383913877424036476406214950912
};

#define DatumGetHugebase62(X)		((hugebase62 *) DatumGetPointer(X))
#define Hugebase62GetDatum(X)		PointerGetDatum(X)
#define PG_GETARG_HUGEBASE62_P(n)	DatumGetHugebase62(PG_GETARG_DATUM(n))
#define PG_RETURN_HUGEBASE62_P(x)	return Hugebase62GetDatum(x)

PG_FUNCTION_INFO_V1(hugebase62_in);
PG_FUNCTION_INFO_V1(hugebase62_out);
PG_FUNCTION_INFO_V1(hugebase62_recv);
PG_FUNCTION_INFO_V1(hugebase62_send);

PG_FUNCTION_INFO_V1(hugebase62_cast_from_text);
PG_FUNCTION_INFO_V1(hugebase62_cast_to_text);
PG_FUNCTION_INFO_V1(hugebase62_cast_from_bytea);
PG_FUNCTION_INFO_V1(hugebase62_cast_to_bytea);

PG_FUNCTION_INFO_V1(hugebase62_eq);
PG_FUNCTION_INFO_V1(hugebase62_ne);
PG_FUNCTION_INFO_V1(hugebase62_lt);
PG_FUNCTION_INFO_V1(hugebase62_le);
PG_FUNCTION_INFO_V1(hugebase62_gt);
PG_FUNCTION_INFO_V1(hugebase62_ge);
PG_FUNCTION_INFO_V1(hugebase62_cmp);
PG_FUNCTION_INFO_V1(hash_hugebase62);


/*
 * Calculate missing powers as we cannot define them as literals
 */
static inline void
calc_hugebase62_powers(int pow_idx)
{
	// We don't need to calculate powers up to pow_idx if it isn't zero
	if (hugebase62_powers[pow_idx] != 0ULL)
		return;

	for (int cur_pow_idx = HUGEBASE62_NONZERO_POWERS; cur_pow_idx <= pow_idx; cur_pow_idx++)
	{
		if (hugebase62_powers[cur_pow_idx] == 0ULL)
			continue;
		hugebase62_powers[cur_pow_idx] = hugebase62_powers[cur_pow_idx - 1] * BASE62_BASE;
	}
}

static inline hugebase62 *
hugebase62_from_str(const char *str)
{
	int			i = 0,
				d = 0,
				n = strlen(str);
	hugebase62	res = 0;
	bool		neg_sign = false;
	hugebase62 *resp;

	if (n == 0){
		OUTOFRANGE_ERROR(str, "hugebase62");
	}
	else if(str[0] == '-')
	{
		if (n - 1 > HUGEBASE62_LENGTH)
			OUTOFRANGE_ERROR(str, "hugebase62");

		neg_sign = true;
		i = 1;
	}
	else if (n > HUGEBASE62_LENGTH)
	{
		OUTOFRANGE_ERROR(str, "hugebase62");
	}

	calc_hugebase62_powers(n - i - 1);

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
					 errmsg("value \"%c\" is not a valid digit for type hugebase62", str[i])));

		res += d * hugebase62_powers[n - i - 1];

		if (res < 0)
			OUTOFRANGE_ERROR(str, "hugebase62");
	}
	if (neg_sign)
		res = 0 - res;

	resp = (hugebase62 *) palloc(sizeof(hugebase62));
	memcpy(resp, &res, sizeof(hugebase62));

	return resp;
}

static inline char *
hugebase62_to_str(hugebase62 *c)
{
	int			i,
				d,
				p = 0;
	hugebase62	m = Abs(*c);
	bool		discard = true;
	char	   *str = palloc0((HUGEBASE62_LENGTH + 2) * sizeof(char));

	if (c < 0)
		str[p++] = '-';

	calc_hugebase62_powers(HUGEBASE62_LENGTH - 1);

	for (i = HUGEBASE62_LENGTH - 1; i >= 0; i--)
	{
		d = m / hugebase62_powers[i];
		m = m - hugebase62_powers[i] * d;

		discard = discard && (d == 0 && i > 0);

		if (!discard)
			str[p++] = base62_digits[d];
	}

	return str;
}

Datum
hugebase62_in(PG_FUNCTION_ARGS)
{
	char	   *str = PG_GETARG_CSTRING(0);

	PG_RETURN_HUGEBASE62_P(hugebase62_from_str(str));
}

Datum
hugebase62_out(PG_FUNCTION_ARGS)
{
	hugebase62 *c = PG_GETARG_HUGEBASE62_P(0);

	PG_RETURN_CSTRING(hugebase62_to_str(c));
}

Datum
hugebase62_recv(PG_FUNCTION_ARGS)
{
}

Datum
hugebase62_send(PG_FUNCTION_ARGS)
{
}

Datum
hugebase62_cast_from_text(PG_FUNCTION_ARGS)
{
	text	   *txt = PG_GETARG_TEXT_P(0);

	PG_RETURN_HUGEBASE62_P(hugebase62_from_str(text_to_cstring(txt)));
}

Datum
hugebase62_cast_to_text(PG_FUNCTION_ARGS)
{
	hugebase62 *c  = PG_GETARG_HUGEBASE62_P(0);

	PG_RETURN_TEXT_P(cstring_to_text(hugebase62_to_str(c)));
}

Datum
hugebase62_cast_from_bytea(PG_FUNCTION_ARGS)
{
}

Datum
hugebase62_cast_to_bytea(PG_FUNCTION_ARGS)
{
}

Datum
hugebase62_eq(PG_FUNCTION_ARGS)
{
}

Datum
hugebase62_ne(PG_FUNCTION_ARGS)
{
}

Datum
hugebase62_lt(PG_FUNCTION_ARGS)
{
}

Datum
hugebase62_le(PG_FUNCTION_ARGS)
{
}

Datum
hugebase62_gt(PG_FUNCTION_ARGS)
{
}

Datum
hugebase62_ge(PG_FUNCTION_ARGS)
{
}

Datum
hugebase62_cmp(PG_FUNCTION_ARGS)
{
}

Datum
hash_hugebase62(PG_FUNCTION_ARGS)
{
}
