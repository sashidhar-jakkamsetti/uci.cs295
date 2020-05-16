.PHONY: all syringePumpSim clean

# gcc is the current compiler
CC=gcc
# options being passed to the above comipler
CFLAGS=-Wall

all: syringePumpSim

syringePumpSim:
	$(CC) $(CFLAGS) syringePumpSim.c -o syringePumpSim.o

clean:
	rm *.o