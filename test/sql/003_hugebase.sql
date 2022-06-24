-- Test function
create or replace function base62_encode(num bigint) 
returns text
as $$
declare
    k_base        integer := 62;
    k_alphabet    text[] := string_to_array( '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz'::text, null);
    
    v_return_text text := '';
    v_remainder   integer;
begin
    loop
        v_remainder := num % k_base;
        num := num / k_base;
        v_return_text := '' || k_alphabet[(v_remainder + 1)] || v_return_text;
    exit when num <= 0;
    end loop ;
    return v_return_text;
end;$$
language plpgsql immutable;

select '\x78000000000000000000000000000000'::bytea::hugebase62;
select '1w'::hugebase62;
select '1W'::hugebase62;
select '3caaaaaaaaaaaaa'::hugebase62;
select '2LKcb1'::hugebase62;
select '\xffffff7f000000000000000000000000'::bytea::hugebase62;
select '2lkcb1'::hugebase62::bytea;
select 'AzL8n0Y58m7aa'::hugebase62;
select 'azl8n0y58m7aa'::hugebase62;
-- ERROR: not a valid digit
select '3&'::hugebase62;
select 'a'::hugebase62 > 'b'::hugebase62;
select 'a'::hugebase62 < 'b'::hugebase62;

-- Test values
select base62_encode(i)::base62
from generate_series(1, 80) g(i);
