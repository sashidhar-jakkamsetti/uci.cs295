.PHONY: all dfa source syringePumpSim syringePumpSimInst clean

# gcc is the current compiler
CC=gcc
# options being passed to the above comipler
CFLAGS=-Wall

PC=python

all: dfa source syringePumpSimInst

dfa:
	$(CC) $(CFLAGS) dfa.c sha224-256.c sha1.c sha384-512.c usha.c ra.c -o dfa.o

source: syringePumpSim
	$(CC) $(CFLAGS) source.c dfa_stub.c -o source.o

syringePumpSimInst:
	$(PC) instrumenter.py syringePumpSim.c source.c

syringePumpSim:
	$(CC) $(CFLAGS) syringePumpSimOriginal.c -o syringePumpSimOriginal.o

clean:
	rm *.o *.out