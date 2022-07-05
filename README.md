![CI](https://github.com/adjust/pg-base62/workflows/CI/badge.svg)

# pg-base62
Base62 extension for PostgreSQL. The extension provides data types to encode and
decode values using base62 encoding scheme.

Base62 uses 62 ASCII characters: `0 - 9`, `A - Z` and `a - z`.

More information about base62 encoding scheme:
- Wikipedia page: https://en.wikipedia.org/wiki/Base62
- A blog post: https://helloacm.com/base62/

## Types

### `base62`

`base62` stores encoded value in 4 bytes integer. The maximum length of string
representation is 6 characters and maximum value of numeric representation is
2147483647.

```sql
=# select '111111'::base62::int;
   int4    
-----------
 931151403

=# select '1111111'::base62::int;
ERROR:  value "1111111" is out of range for type base62

=# select '2lkcb'::base62::int, '2lkcb'::base62, '2LKCB'::base62::int, '2LKCB'::base62;
   int4   | base62 |   int4   | base62 
----------+--------+----------+--------
 40933305 | 2lkcb  | 34635195 | 2LKCB
```

### `bigbase62`

`bigbase62` stores encoded value in 8 bytes integer. The maxiumum length of
string representation is 11 characters and the maximum value of numeric
representation is 9223372036854775807.

```sql
=# select '11111111111'::bigbase62::bigint;
        int8        
--------------------
 853058371866181867

=# select '111111111111'::bigbase62::bigint;
ERROR:  value "111111111111" is out of range for type bigbase62

=# select '2lkcb1'::bigbase62::bigint, '2lkcb1'::bigbase62, '2LKCB1'::bigbase62::bigint, '2LKCB1'::bigbase62;
    int8    | bigbase62 |    int8    | bigbase62 
------------+-----------+------------+-----------
 2537864911 | 2lkcb1    | 2147382091 | 2LKCB1
```

### `hugebase62`

`hugebase62` stores encoded value in 16 bytes integer. The maximum length of
string representation is 20 characters.

```sql
=# select '11111111111111111111'::hugebase62;
      hugebase62      
----------------------
 11111111111111111111

=# select '111111111111111111111'::hugebase62;
ERROR:  value "111111111111111111111" is out of range for type hugebase62

=# select '2lkcb1'::hugebase62::bytea, '2lkcb1'::hugebase62, '2LKCB1'::hugebase62::bytea, '2LKCB1'::hugebase62;
               bytea                | hugebase62 |               bytea                | hugebase62 
------------------------------------+------------+------------------------------------+------------
 \xcfbe4497000000000000000000000000 | 2lkcb1     | \x4b73fe7f000000000000000000000000 | 2LKCB1

=# select '\xcfbe4497000000000000000000000000'::bytea::hugebase62;
 hugebase62 
------------
 2lkcb1
```
