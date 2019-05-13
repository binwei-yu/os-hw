.PHONY: clean all

CC = gcc
CFLAG = -fPIC

all: counter_test.o list_insertion_test.o hash_insertion_test.o hash_scaling_test.o

libspin.so: spin.c
	@$(CC) -c $^ $(CFLAG) -o $@

libcounter.so: counter.c
	@$(CC) -c $^ $(CFLAG) -o $@

liblist.so: list.c
	@$(CC) -c $^ $(CFLAG) -o $@

libhash.so: hash.c
	@$(CC) -c $^ $(CFLAG) -o $@

LIBS = libspin.so libcounter.so liblist.so libhash.so

counter_test.o: counter_test.c $(LIBS)
	@$(CC) -g -Wall -o $@ $^

list_insertion_test.o: list_insertion_test.c $(LIBS)
	@$(CC) -g -Wall -o $@ $^

hash_insertion_test.o: hash_insertion_test.c $(LIBS)
	@$(CC) -g -Wall -o $@ $^

hash_scaling_test.o: hash_scaling_test.c $(LIBS)
	@$(CC) -g -Wall -o $@ $^

clean:
	@rm *.so *.o