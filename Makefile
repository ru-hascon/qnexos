CC = gcc
CFLAGS = -std=c11 -O3 -march=native -mtune=native -Wall -Wextra -pedantic
LDFLAGS = -lm

all: test_trit test_regulator

test_trit: tests/test_trit_packer.c src/tritpacker/trit_packer.c
$(CC) $(CFLAGS) -Isrc/tritpacker $^ -o $@ $(LDFLAGS)
./$@

test_regulator: tests/test_regulator.c src/toric/qnex_torus_regulator_opt.c src/tritpacker/trit_packer.c
$(CC) $(CFLAGS) -Isrc/tritpacker $^ -o $@ $(LDFLAGS)
./$@

clean:
rm -f test_* benchmark_*
