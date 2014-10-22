
.PHONY: clean

CC=gcc
CFLAGS=-Wfatal-errors -Ofast -Wall -Wno-unused-variable -mtune=native -march=native
FILES=mt_rand.c entry.c sub_table.c dyn_perf.c
test_files=test.c main.c

test:
	$(CC) $(CFLAGS) $(FILES) $(test_files) -o test_64
	$(MAKE) clean
	$(CC) $(CFLAGS) $(FILES) $(test_files) -DENTRY_KEY_T="unsigned" -o test_32
	$(MAKE) clean
	$(CC) $(CFLAGS) $(FILES) $(test_files) -DENTRY_KEY_T="unsigned short" -o test_16
	$(MAKE) clean
	./test_16
	./test_32
	./test_64
	rm -f test_16 test_32 test_64

shared:
	$(CC) $(CFLAGS) $(FILES) -fPIC -shared -o libdyn_perf.so

clean:
	rm -f *.out *.o *.so
