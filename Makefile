EXTENSION = hashlib
MODULE_big = hashlib
DATA = sql/hashlib--0.0.1.sql
OBJS = src/cityhash64.o src/cityhash128.o src/crc32.o src/lookup2.o src/lookup3be.o src/lookup3le.o src/murmur.o src/siphash24.o src/spookyhash.o src/xxhash.o
PG_CONFIG = pg_config

# PGXN variables
DISTVERSION = $(shell grep -m 1 '"version":' META.json | sed -e 's/[[:space:]]*"version":[[:space:]]*"\([^"]*\)",\{0,1\}/\1/')

# Test configuration
TESTS = $(wildcard tests/sql/*.sql)
REGRESS = $(patsubst tests/sql/%.sql,%,$(TESTS))
REGRESS_OPTS = --inputdir=tests --load-extension=$(EXTENSION)

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

# Custom test target that shows diffs on failure
test:
	@$(MAKE) --silent installcheck || (echo "=== DIFF ===" && cat regression.diffs 2>/dev/null && exit 1)

# PGXN distribution target
dist:
	git archive --format zip --prefix=$(EXTENSION)-$(DISTVERSION)/ -o $(EXTENSION)-$(DISTVERSION).zip HEAD

# Validate META.json for PGXN
validate-meta:
	@command -v pgxn >/dev/null 2>&1 && pgxn validate-meta || echo "pgxn client not found, skipping validation"

.PHONY: test dist validate-meta