EXTENSION = base62
MODULE_big = base62

PG_CONFIG ?= pg_config
DATA = $(wildcard *--*.sql)
OBJS = $(patsubst %.c,%.o,$(wildcard src/*.c))

PGXS := $(shell $(PG_CONFIG) --pgxs)

TESTS = $(sort $(wildcard test/sql/*.sql))
REGRESS = $(patsubst test/sql/%.sql,%,$(TESTS))
REGRESS_OPTS = --inputdir=test --outputdir=test

include $(PGXS)
