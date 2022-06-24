select 120::bigint::bigbase62;
select '1w'::bigbase62::bigint;
select '1W'::bigbase62::bigint;
-- ERROR: out of range
select '3caaaaaaaaaaaaa'::bigbase62::bigint;
select '2LKcb1'::bigbase62::bigint;
select 2147483647::bigint::bigbase62;
select '2lkcb1'::bigbase62::bigint;
select 9223372036854775807::bigbase62;
select 'AzL8n0Y58m7'::bigbase62;
-- ERROR: not a valid digit
select '3&'::bigbase62::bigint;
select 'a'::bigbase62 > 'b'::bigbase62;
select 'a'::bigbase62 < 'b'::bigbase62;

-- Test values
select i::bigint::bigbase62
from generate_series(1, 80) g(i);
