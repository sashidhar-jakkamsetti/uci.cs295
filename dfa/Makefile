.PHONY: all dfa syringePumpSim clean

# gcc is the current compiler
CC=gcc
# options being passed to the above comipler
CFLAGS=-Wall

all: dfa syringePumpSim

dfa:
	$(CC) $(CFLAGS) dfa.c -o dfa.o

syringePumpSim:
	$(CC) $(CFLAGS) syringePumpSim.c dfa_stub.c -o syringePumpSim.o

clean:
	rm *.o