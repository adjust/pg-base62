set max_parallel_workers_per_gather=4;
set force_parallel_mode=on;
set parallel_setup_cost = 10;
set parallel_tuple_cost = 0.001;

create table parallel_test(i int, b1 base62, b2 bigbase62, b3 hugebase62) with (parallel_workers = 4);
insert into parallel_test (i, b1, b2, b3)
select i, i::int, i::bigint, i::text::hugebase62
from generate_series(1,1e4) i;

explain (costs off,verbose)
select count(*) from parallel_test where b1 = '1ftese';

explain (costs off,verbose)
select count(*) from parallel_test where b2 = '1ftese';

explain (costs off,verbose)
select count(*) from parallel_test where b3 = '100000';

explain (costs off,verbose)
select b1, count(*) from parallel_test group by 1;
