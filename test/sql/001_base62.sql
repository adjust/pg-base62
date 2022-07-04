create extension base62;

select 120::base62;
select '1w'::base62::int;
select '1W'::base62::int;
select '2LKcb1'::base62::int;
select 2147483647::base62;
-- ERROR: out of range
select 'AAAAAA'::base62;
-- ERROR: out of range
select 'aaaaaa'::base62;
-- ERROR: out of range
select '3caaaaaaaaaaaaa'::base62::bigint;
-- ERROR: out of range
select '2lkcb1'::base62::int;
-- ERROR: not a valid digit
select '3&'::base62::bigint;

select 'a'::base62 > 'b'::base62;
select 'a'::base62 < 'b'::base62;

-- Negativity test
select i::base62, i::base62::int from (values ('11111'), ('acDeq'), ('10'), ('A'), ('-1'), ('-A')) as v(i);

-- Test values
select i::base62
from generate_series(1, 80) g(i);
