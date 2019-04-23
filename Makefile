CC = gcc

main: main.c
	$(CC) main.c -o main 

all: main

clean:
	rm main record.txt