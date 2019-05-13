.PHONY: clean all

CC = gcc
CFLAG = -fPIC

all: main

libspin.so: spin.c
	@$(CC) -c $^ $(CFLAG) -o $@

libcounter.so: counter.c
	@$(CC) -c $^ $(CFLAG) -o $@

liblist.so: list.c
	@$(CC) -c $^ $(CFLAG) -o $@

libhash.so: hash.c
	@$(CC) -c $^ $(CFLAG) -o $@

main: main.c libspin.so libcounter.so liblist.so libhash.so
	@$(CC) -g -Wall -o main main.c libspin.so libcounter.so liblist.so libhash.so

clean:
	@rm *.so main