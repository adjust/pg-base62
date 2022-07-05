select '\x78000000000000000000000000000000'::bytea::hugebase62;
select '1w'::hugebase62;
select '1W'::hugebase62;
select '3caaaaaaaaaaaaa'::hugebase62;
select '2LKcb1'::hugebase62;
select '\xffffff7f000000000000000000000000'::bytea::hugebase62;
select '2lkcb1'::hugebase62::bytea;
select 'AzL8n0Y58m7aa'::hugebase62;
select 'azl8n0y58m7aa'::hugebase62;
select 'AAAAAAAAAAAAAAAAAAAA'::hugebase62;
select 'aaaaaaaaaaaaaaaaaaaa'::hugebase62;
-- ERROR: out of range
select 'AAAAAAAAAAAAAAAAAAAAA'::hugebase62;
-- ERROR: not a valid digit
select '3&'::hugebase62;
select 'a'::hugebase62 > 'b'::hugebase62;
select 'a'::hugebase62 < 'b'::hugebase62;

-- Negativity test
select i::text::hugebase62, i::text::hugebase62::bytea from (values ('11111'), ('acDeq'), ('10'), ('A'), ('-1'), ('-A')) as v(i);

-- Test values
select bigbase62_encode(i)::base62
from generate_series(1, 80) g(i);
