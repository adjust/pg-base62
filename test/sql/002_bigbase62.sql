select 120::bigint::bigbase62;
select '1w'::bigbase62::bigint;
select '1W'::bigbase62::bigint;
select '2LKcb1'::bigbase62::bigint;
select 2147483647::bigint::bigbase62;
select '2lkcb1'::bigbase62::bigint;
select 9223372036854775807::bigbase62;
select 'AzL8n0Y58m7'::bigbase62;
-- ERROR: out of range
select 'AAAAAAAAAAA'::base62;
-- ERROR: out of range
select 'aaaaaaaaaaa'::base62;
-- ERROR: out of range
select '3caaaaaaaaaaaaa'::bigbase62::bigint;
-- ERROR: not a valid digit
select '3&'::bigbase62::bigint;

select 'a'::bigbase62 > 'b'::bigbase62;
select 'a'::bigbase62 < 'b'::bigbase62;

-- Negativity test
select  i::bigbase62,i::bigbase62::bigint from (values ('11111'), ('acDeq'), ('10'), ('A'), ('-1'), ('-A')) as v(i);

-- Test values
select i::bigint::bigbase62
from generate_series(1, 80) g(i);
