.PHONY: all dfa source clean

# gcc is the current compiler
CC=gcc
# options being passed to the above comipler
CFLAGS=-Wall

all: dfa source

dfa:
	$(CC) $(CFLAGS) dfa.c sha224-256.c sha1.c sha384-512.c usha.c ra.c -o dfa.o

source:
	$(CC) $(CFLAGS) source.c dfa_stub.c -o source.o

clean:
	rm *.o *.out