-- Definition of base62

CREATE FUNCTION base62_in(cstring) RETURNS base62
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION base62_out(base62) RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION base62_recv(internal) RETURNS base62
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'int4recv';

CREATE FUNCTION base62_send(base62) RETURNS bytea
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'int4send';

CREATE TYPE base62 (
    INPUT          = base62_in,
    OUTPUT         = base62_out,
    RECEIVE        = base62_recv,
    SEND           = base62_send,
    LIKE           = integer,
    CATEGORY       = 'N'
);
COMMENT ON TYPE base62 IS 'int written in base62: [0-9A-Za-z]+';

CREATE FUNCTION base62(text) RETURNS base62
AS 'MODULE_PATHNAME', 'base62_cast_from_text'
LANGUAGE C
IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION text(base62) RETURNS text
AS 'MODULE_PATHNAME', 'base62_cast_to_text'
LANGUAGE C
IMMUTABLE STRICT PARALLEL SAFE;

CREATE CAST (text as base62) WITH FUNCTION base62(text) AS IMPLICIT;
CREATE CAST (base62 as text) WITH FUNCTION text(base62);

CREATE CAST (integer as base62) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (base62 as integer) WITHOUT FUNCTION AS IMPLICIT;

-- Definition of base62 operators

CREATE FUNCTION base62_eq(base62, base62) RETURNS boolean
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'int4eq';

CREATE FUNCTION base62_ne(base62, base62) RETURNS boolean
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'int4ne';

CREATE FUNCTION base62_lt(base62, base62) RETURNS boolean
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'int4lt';

CREATE FUNCTION base62_le(base62, base62) RETURNS boolean
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'int4le';

CREATE FUNCTION base62_gt(base62, base62) RETURNS boolean
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'int4gt';

CREATE FUNCTION base62_ge(base62, base62) RETURNS boolean
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'int4ge';

CREATE FUNCTION base62_cmp(base62, base62) RETURNS integer
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'btint4cmp';

CREATE FUNCTION hash_base62(base62) RETURNS integer
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'hashint4';

CREATE OPERATOR = (
    LEFTARG = base62,
    RIGHTARG = base62,
    PROCEDURE = base62_eq,
    COMMUTATOR = '=',
    NEGATOR = '<>',
    RESTRICT = eqsel,
    JOIN = eqjoinsel
);
COMMENT ON OPERATOR =(base62, base62) IS 'equals';

CREATE OPERATOR <> (
    LEFTARG = base62,
    RIGHTARG = base62,
    PROCEDURE = base62_ne,
    COMMUTATOR = '<>',
    NEGATOR = '=',
    RESTRICT = neqsel,
    JOIN = neqjoinsel
);
COMMENT ON OPERATOR <>(base62, base62) IS 'not equals';

CREATE OPERATOR < (
    LEFTARG = base62,
    RIGHTARG = base62,
    PROCEDURE = base62_lt,
    COMMUTATOR = > ,
    NEGATOR = >= ,
    RESTRICT = scalarltsel,
    JOIN = scalarltjoinsel
);
COMMENT ON OPERATOR <(base62, base62) IS 'less-than';

CREATE OPERATOR <= (
    LEFTARG = base62,
    RIGHTARG = base62,
    PROCEDURE = base62_le,
    COMMUTATOR = >= ,
    NEGATOR = > ,
    RESTRICT = scalarltsel,
    JOIN = scalarltjoinsel
);
COMMENT ON OPERATOR <=(base62, base62) IS 'less-than-or-equal';

CREATE OPERATOR > (
    LEFTARG = base62,
    RIGHTARG = base62,
    PROCEDURE = base62_gt,
    COMMUTATOR = < ,
    NEGATOR = <= ,
    RESTRICT = scalargtsel,
    JOIN = scalargtjoinsel
);
COMMENT ON OPERATOR >(base62, base62) IS 'greater-than';

CREATE OPERATOR >= (
    LEFTARG = base62,
    RIGHTARG = base62,
    PROCEDURE = base62_ge,
    COMMUTATOR = <= ,
    NEGATOR = < ,
    RESTRICT = scalargtsel,
    JOIN = scalargtjoinsel
);
COMMENT ON OPERATOR >=(base62, base62) IS 'greater-than-or-equal';

CREATE OPERATOR CLASS btree_base62_ops
DEFAULT FOR TYPE base62 USING btree
AS
        OPERATOR        1       <  ,
        OPERATOR        2       <= ,
        OPERATOR        3       =  ,
        OPERATOR        4       >= ,
        OPERATOR        5       >  ,
        FUNCTION        1       base62_cmp(base62, base62);

CREATE OPERATOR CLASS hash_base62_ops
    DEFAULT FOR TYPE base62 USING hash AS
        OPERATOR        1       = ,
        FUNCTION        1       hash_base62(base62);

-- Definition of bigbase62

CREATE FUNCTION bigbase62_in(cstring) RETURNS bigbase62
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION bigbase62_out(bigbase62) RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION bigbase62_recv(internal) RETURNS bigbase62
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'int8recv';

CREATE FUNCTION bigbase62_send(bigbase62) RETURNS bytea
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'int8send';

CREATE TYPE bigbase62 (
    INPUT          = bigbase62_in,
    OUTPUT         = bigbase62_out,
    RECEIVE        = bigbase62_recv,
    SEND           = bigbase62_send,
    LIKE           = bigint,
    CATEGORY       = 'N'
);
COMMENT ON TYPE bigbase62 IS 'bigint written in bigbase62: [0-9A-Za-z]+';

CREATE FUNCTION bigbase62(text) RETURNS bigbase62
AS 'MODULE_PATHNAME', 'bigbase62_cast_from_text'
LANGUAGE C
IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION text(bigbase62) RETURNS text
AS 'MODULE_PATHNAME', 'bigbase62_cast_to_text'
LANGUAGE C
IMMUTABLE STRICT PARALLEL SAFE;

CREATE CAST (text as bigbase62) WITH FUNCTION bigbase62(text) AS IMPLICIT;
CREATE CAST (bigbase62 as text) WITH FUNCTION text(bigbase62);

CREATE CAST (bigint as bigbase62) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (bigbase62 as bigint) WITHOUT FUNCTION AS IMPLICIT;

-- Definition of bigbase62 operators

CREATE FUNCTION bigbase62_eq(bigbase62, bigbase62) RETURNS boolean
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'int8eq';

CREATE FUNCTION bigbase62_ne(bigbase62, bigbase62) RETURNS boolean
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'int8ne';

CREATE FUNCTION bigbase62_lt(bigbase62, bigbase62) RETURNS boolean
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'int8lt';

CREATE FUNCTION bigbase62_le(bigbase62, bigbase62) RETURNS boolean
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'int8le';

CREATE FUNCTION bigbase62_gt(bigbase62, bigbase62) RETURNS boolean
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'int8gt';

CREATE FUNCTION bigbase62_ge(bigbase62, bigbase62) RETURNS boolean
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'int8ge';

CREATE FUNCTION bigbase62_cmp(bigbase62, bigbase62) RETURNS integer
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'btint8cmp';

CREATE FUNCTION hash_bigbase62(bigbase62) RETURNS integer
LANGUAGE internal
IMMUTABLE PARALLEL SAFE
AS 'hashint8';

CREATE OPERATOR = (
    LEFTARG = bigbase62,
    RIGHTARG = bigbase62,
    PROCEDURE = bigbase62_eq,
    COMMUTATOR = '=',
    NEGATOR = '<>',
    RESTRICT = eqsel,
    JOIN = eqjoinsel
);
COMMENT ON OPERATOR =(bigbase62, bigbase62) IS 'equals';

CREATE OPERATOR <> (
    LEFTARG = bigbase62,
    RIGHTARG = bigbase62,
    PROCEDURE = bigbase62_ne,
    COMMUTATOR = '<>',
    NEGATOR = '=',
    RESTRICT = neqsel,
    JOIN = neqjoinsel
);
COMMENT ON OPERATOR <>(bigbase62, bigbase62) IS 'not equals';

CREATE OPERATOR < (
    LEFTARG = bigbase62,
    RIGHTARG = bigbase62,
    PROCEDURE = bigbase62_lt,
    COMMUTATOR = > ,
    NEGATOR = >= ,
    RESTRICT = scalarltsel,
    JOIN = scalarltjoinsel
);
COMMENT ON OPERATOR <(bigbase62, bigbase62) IS 'less-than';

CREATE OPERATOR <= (
    LEFTARG = bigbase62,
    RIGHTARG = bigbase62,
    PROCEDURE = bigbase62_le,
    COMMUTATOR = >= ,
    NEGATOR = > ,
    RESTRICT = scalarltsel,
    JOIN = scalarltjoinsel
);
COMMENT ON OPERATOR <=(bigbase62, bigbase62) IS 'less-than-or-equal';

CREATE OPERATOR > (
    LEFTARG = bigbase62,
    RIGHTARG = bigbase62,
    PROCEDURE = bigbase62_gt,
    COMMUTATOR = < ,
    NEGATOR = <= ,
    RESTRICT = scalargtsel,
    JOIN = scalargtjoinsel
);
COMMENT ON OPERATOR >(bigbase62, bigbase62) IS 'greater-than';

CREATE OPERATOR >= (
    LEFTARG = bigbase62,
    RIGHTARG = bigbase62,
    PROCEDURE = bigbase62_ge,
    COMMUTATOR = <= ,
    NEGATOR = < ,
    RESTRICT = scalargtsel,
    JOIN = scalargtjoinsel
);
COMMENT ON OPERATOR >=(bigbase62, bigbase62) IS 'greater-than-or-equal';

CREATE OPERATOR CLASS btree_bigbase62_ops
DEFAULT FOR TYPE bigbase62 USING btree
AS
        OPERATOR        1       <  ,
        OPERATOR        2       <= ,
        OPERATOR        3       =  ,
        OPERATOR        4       >= ,
        OPERATOR        5       >  ,
        FUNCTION        1       bigbase62_cmp(bigbase62, bigbase62);

CREATE OPERATOR CLASS hash_bigbase62_ops
    DEFAULT FOR TYPE bigbase62 USING hash AS
        OPERATOR        1       = ,
        FUNCTION        1       hash_bigbase62(bigbase62);

-- Definition of hugebase62

CREATE FUNCTION hugebase62_in(cstring) RETURNS hugebase62
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION hugebase62_out(hugebase62) RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION hugebase62_recv(internal) RETURNS hugebase62
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE PARALLEL SAFE;

CREATE FUNCTION hugebase62_send(hugebase62) RETURNS bytea
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE PARALLEL SAFE;

CREATE TYPE hugebase62 (
    INPUT          = hugebase62_in,
    OUTPUT         = hugebase62_out,
    RECEIVE        = hugebase62_recv,
    SEND           = hugebase62_send,
    INTERNALLENGTH = 16,
    CATEGORY       = 'N'
);
COMMENT ON TYPE hugebase62 IS '128 bit integer written in hugebase62: [0-9A-Za-z]+';

CREATE FUNCTION hugebase62(text) RETURNS hugebase62
AS 'MODULE_PATHNAME', 'hugebase62_cast_from_text'
LANGUAGE C
IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION text(hugebase62) RETURNS text
AS 'MODULE_PATHNAME', 'hugebase62_cast_to_text'
LANGUAGE C
IMMUTABLE STRICT PARALLEL SAFE;

CREATE CAST (text as hugebase62) WITH FUNCTION hugebase62(text) AS IMPLICIT;
CREATE CAST (hugebase62 as text) WITH FUNCTION text(hugebase62);

CREATE CAST (bigint as hugebase62) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (hugebase62 as bigint) WITHOUT FUNCTION AS IMPLICIT;

-- Definition of hugebase62 operators

CREATE FUNCTION hugebase62_eq(hugebase62, hugebase62) RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE PARALLEL SAFE;

CREATE FUNCTION hugebase62_ne(hugebase62, hugebase62) RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE PARALLEL SAFE;

CREATE FUNCTION hugebase62_lt(hugebase62, hugebase62) RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE PARALLEL SAFE;

CREATE FUNCTION hugebase62_le(hugebase62, hugebase62) RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE PARALLEL SAFE;

CREATE FUNCTION hugebase62_gt(hugebase62, hugebase62) RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE PARALLEL SAFE;

CREATE FUNCTION hugebase62_ge(hugebase62, hugebase62) RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE PARALLEL SAFE;

CREATE FUNCTION hugebase62_cmp(hugebase62, hugebase62) RETURNS integer
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE PARALLEL SAFE;

CREATE FUNCTION hash_hugebase62(hugebase62) RETURNS integer
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE PARALLEL SAFE;

CREATE OPERATOR = (
    LEFTARG = hugebase62,
    RIGHTARG = hugebase62,
    PROCEDURE = hugebase62_eq,
    COMMUTATOR = '=',
    NEGATOR = '<>',
    RESTRICT = eqsel,
    JOIN = eqjoinsel
);
COMMENT ON OPERATOR =(hugebase62, hugebase62) IS 'equals';

CREATE OPERATOR <> (
    LEFTARG = hugebase62,
    RIGHTARG = hugebase62,
    PROCEDURE = hugebase62_ne,
    COMMUTATOR = '<>',
    NEGATOR = '=',
    RESTRICT = neqsel,
    JOIN = neqjoinsel
);
COMMENT ON OPERATOR <>(hugebase62, hugebase62) IS 'not equals';

CREATE OPERATOR < (
    LEFTARG = hugebase62,
    RIGHTARG = hugebase62,
    PROCEDURE = hugebase62_lt,
    COMMUTATOR = > ,
    NEGATOR = >= ,
    RESTRICT = scalarltsel,
    JOIN = scalarltjoinsel
);
COMMENT ON OPERATOR <(hugebase62, hugebase62) IS 'less-than';

CREATE OPERATOR <= (
    LEFTARG = hugebase62,
    RIGHTARG = hugebase62,
    PROCEDURE = hugebase62_le,
    COMMUTATOR = >= ,
    NEGATOR = > ,
    RESTRICT = scalarltsel,
    JOIN = scalarltjoinsel
);
COMMENT ON OPERATOR <=(hugebase62, hugebase62) IS 'less-than-or-equal';

CREATE OPERATOR > (
    LEFTARG = hugebase62,
    RIGHTARG = hugebase62,
    PROCEDURE = hugebase62_gt,
    COMMUTATOR = < ,
    NEGATOR = <= ,
    RESTRICT = scalargtsel,
    JOIN = scalargtjoinsel
);
COMMENT ON OPERATOR >(hugebase62, hugebase62) IS 'greater-than';

CREATE OPERATOR >= (
    LEFTARG = hugebase62,
    RIGHTARG = hugebase62,
    PROCEDURE = hugebase62_ge,
    COMMUTATOR = <= ,
    NEGATOR = < ,
    RESTRICT = scalargtsel,
    JOIN = scalargtjoinsel
);
COMMENT ON OPERATOR >=(hugebase62, hugebase62) IS 'greater-than-or-equal';

CREATE OPERATOR CLASS btree_hugebase62_ops
DEFAULT FOR TYPE hugebase62 USING btree
AS
        OPERATOR        1       <  ,
        OPERATOR        2       <= ,
        OPERATOR        3       =  ,
        OPERATOR        4       >= ,
        OPERATOR        5       >  ,
        FUNCTION        1       hugebase62_cmp(hugebase62, hugebase62);

CREATE OPERATOR CLASS hash_hugebase62_ops
    DEFAULT FOR TYPE hugebase62 USING hash AS
        OPERATOR        1       = ,
        FUNCTION        1       hash_hugebase62(hugebase62);
