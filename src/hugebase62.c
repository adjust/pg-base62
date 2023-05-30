#include "base62.h"
#include "fmgr.h"

#include "lib/stringinfo.h"
#include "libpq/pqformat.h"
#include "utils/builtins.h"

#if PG_VERSION_NUM >= 130000
#include "common/hashfn.h"
#elif PG_VERSION_NUM >= 120000
#include "utils/hashutils.h"
#else
#include "access/hash.h"
#endif

#define HUGEBASE62_NONZERO_POWERS	11
// int128 can store 22 characters, to avoid overflow the capacity is lower
#define HUGEBASE62_LENGTH			20

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
};

#define CHECK_HUGEBASE62_SIZE(nbytes)										\
	if ((nbytes) != sizeof(hugebase62))										\
		ereport(ERROR,														\
				(errmsg("received incorrect length (expected %ld bytes, got %d)", \
						sizeof(hugebase62), (nbytes))));

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
		if (hugebase62_powers[cur_pow_idx] != 0ULL)
			continue;
		hugebase62_powers[cur_pow_idx] = hugebase62_powers[cur_pow_idx - 1] * BASE62_BASE;
	}
}

static inline hugebase62 *
hugebase62_from_str(const char *str)
{
	int			i = 0,
				n = strlen(str);
	hugebase62	res = 0;
	bool		neg_sign = false;
	hugebase62 *resp;

	if (n == 0)
	{
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
		int32		d;
		hugebase62	res_buf;

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

		res_buf = d * hugebase62_powers[n - i - 1];
		if (res_buf < 0)
			OUTOFRANGE_ERROR(str, "hugebase62");

		res += res_buf;

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
	char	   *str = palloc((HUGEBASE62_LENGTH + 2) * sizeof(char));

	if (*c < 0)
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

	// Null terminator
	str[p] = '\0';

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
	StringInfo	buf = (StringInfo) PG_GETARG_POINTER(0);
	int			nbytes;
	hugebase62 *res;

	nbytes = buf->len - buf->cursor;
	CHECK_HUGEBASE62_SIZE(nbytes);

	res = palloc(sizeof(hugebase62));
	pq_copymsgbytes(buf, (char *) res, nbytes);

	PG_RETURN_HUGEBASE62_P(res);
}

Datum
hugebase62_send(PG_FUNCTION_ARGS)
{
	hugebase62 *value = PG_GETARG_HUGEBASE62_P(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendbytes(&buf, (char *) value, sizeof(hugebase62));

	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
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
	bytea	   *value = PG_GETARG_BYTEA_P(0);
	int			nbytes = VARSIZE_ANY_EXHDR(value);
	hugebase62 *res;

	CHECK_HUGEBASE62_SIZE(nbytes);

	res = (hugebase62 *) palloc(sizeof(hugebase62));
	memcpy(res, VARDATA_ANY(value), nbytes);

	PG_RETURN_HUGEBASE62_P(res);
}

Datum
hugebase62_cast_to_bytea(PG_FUNCTION_ARGS)
{
	hugebase62 *value = PG_GETARG_HUGEBASE62_P(0);
	bytea	   *res;

	res = (bytea *) palloc(sizeof(hugebase62) + VARHDRSZ);
	SET_VARSIZE(res, sizeof(hugebase62) + VARHDRSZ);
	memcpy(VARDATA(res), value, sizeof(hugebase62));

	PG_RETURN_BYTEA_P(res);
}

Datum
hugebase62_eq(PG_FUNCTION_ARGS)
{
	hugebase62 *a = PG_GETARG_HUGEBASE62_P(0);
	hugebase62 *b = PG_GETARG_HUGEBASE62_P(1);

	PG_RETURN_BOOL(*a == *b);
}

Datum
hugebase62_ne(PG_FUNCTION_ARGS)
{
	hugebase62 *a = PG_GETARG_HUGEBASE62_P(0);
	hugebase62 *b = PG_GETARG_HUGEBASE62_P(1);

	PG_RETURN_BOOL(*a != *b);
}

Datum
hugebase62_lt(PG_FUNCTION_ARGS)
{
	hugebase62 *a = PG_GETARG_HUGEBASE62_P(0);
	hugebase62 *b = PG_GETARG_HUGEBASE62_P(1);

	PG_RETURN_BOOL(*a < *b);
}

Datum
hugebase62_le(PG_FUNCTION_ARGS)
{
	hugebase62 *a = PG_GETARG_HUGEBASE62_P(0);
	hugebase62 *b = PG_GETARG_HUGEBASE62_P(1);

	PG_RETURN_BOOL(*a <= *b);
}

Datum
hugebase62_gt(PG_FUNCTION_ARGS)
{
	hugebase62 *a = PG_GETARG_HUGEBASE62_P(0);
	hugebase62 *b = PG_GETARG_HUGEBASE62_P(1);

	PG_RETURN_BOOL(*a > *b);
}

Datum
hugebase62_ge(PG_FUNCTION_ARGS)
{
	hugebase62 *a = PG_GETARG_HUGEBASE62_P(0);
	hugebase62 *b = PG_GETARG_HUGEBASE62_P(1);

	PG_RETURN_BOOL(*a >= *b);
}

Datum
hugebase62_cmp(PG_FUNCTION_ARGS)
{
	hugebase62 *a = PG_GETARG_HUGEBASE62_P(0);
	hugebase62 *b = PG_GETARG_HUGEBASE62_P(1);

	if (*a > *b)
		PG_RETURN_INT32(1);
	else if (*a == *b)
		PG_RETURN_INT32(0);
	else
		PG_RETURN_INT32(-1);
}

Datum
hash_hugebase62(PG_FUNCTION_ARGS)
{
	hugebase62 *value = PG_GETARG_HUGEBASE62_P(0);

#if PG_VERSION_NUM >= 130000
	PG_RETURN_INT32(hash_bytes((unsigned char *) value, sizeof(hugebase62)));
#else
	PG_RETURN_INT32(DatumGetInt32(hash_any((unsigned char *) value, sizeof(hugebase62))));
#endif
}
