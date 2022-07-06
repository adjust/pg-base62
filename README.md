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
-- Cast from text
=# select '2LKcb1'::base62::int;
    int4    
------------
 2147483647

-- Cast from integer
=# select 2147483647::base62;
 base62 
--------
 2LKcb1

=# select '1111111'::base62::int;
ERROR:  value "1111111" is out of range for type base62

-- The type is case sensitive
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
-- Cast from text
=# select 'AzL8n0Y58m7'::bigbase62::bigint;
        int8         
---------------------
 9223372036854775807

-- Cast from integer
=# select 9223372036854775807::bigbase62;
  bigbase62  
-------------
 AzL8n0Y58m7

=# select '111111111111'::bigbase62::bigint;
ERROR:  value "111111111111" is out of range for type bigbase62

-- The type is case sensitive
=# select '2lkcb1'::bigbase62::bigint, '2lkcb1'::bigbase62, '2LKCB1'::bigbase62::bigint, '2LKCB1'::bigbase62;
    int8    | bigbase62 |    int8    | bigbase62 
------------+-----------+------------+-----------
 2537864911 | 2lkcb1    | 2147382091 | 2LKCB1
```

### `hugebase62`

`hugebase62` stores encoded value in 16 bytes integer. The maximum length of
string representation is 20 characters.

It doesn't support casting from/to numeric type. Instead it support casting
from/to `bytea` type. This is useful for applications to work with `hugebase62`.

```sql
-- Cast from text
=# select 'AzL8n0Y58m7AzL8n0Y58'::hugebase62;
      hugebase62      
----------------------
 AzL8n0Y58m7AzL8n0Y58

=# select 'AzL8n0Y58m7AzL8n0Y58'::hugebase62::bytea;
               bytea                
------------------------------------
 \x960c06065a6ed8ffff1e7149f40b1800

-- Cast from bytea
=# select '\x960c06065a6ed8ffff1e7149f40b1800'::bytea::hugebase62;
      hugebase62      
----------------------
 AzL8n0Y58m7AzL8n0Y58

=# select '111111111111111111111'::hugebase62;
ERROR:  value "111111111111111111111" is out of range for type hugebase62

-- The type is case sensitive
=# select '2lkcb1'::hugebase62::bytea, '2lkcb1'::hugebase62, '2LKCB1'::hugebase62::bytea, '2LKCB1'::hugebase62;
               bytea                | hugebase62 |               bytea                | hugebase62 
------------------------------------+------------+------------------------------------+------------
 \xcfbe4497000000000000000000000000 | 2lkcb1     | \x4b73fe7f000000000000000000000000 | 2LKCB1
```

### Index support

All types support `btree` and `hash` indexes.

## Installation from source codes

To install `base62`, execute this in the extension's directory:

```shell
make install
```

> **Notice:** Don't forget to set the `PG_CONFIG` variable (`make PG_CONFIG=...`)
> in case you want to test `base62` on a non-default or custom build of PostgreSQL.
> Read more [here](https://wiki.postgresql.org/wiki/Building_and_Installing_PostgreSQL_Extension_Modules).

> **Important:** To compile `base62` your compiler should support 128-bit integers,
> otherwise it will fail during compilation.
