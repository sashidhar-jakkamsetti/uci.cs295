#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>

void report_and_exit(const char* msg) 
{
    perror(msg);
    exit(-1);
}

unsigned writetoSmem(const void* memptr, const unsigned index, const void *datapointer, const int size)
{
    unsigned ipointer = index;
    memcpy((char*)memptr + ipointer, &size, sizeof(size));
    printf("writing %d bytes\n", size);
    ipointer += sizeof(size);
    memcpy((char*)memptr + ipointer, datapointer, size);
    ipointer += size;
    return ipointer;
}

unsigned readfromSmem(const void* memptr, const unsigned index, void *datapointer)
{
    unsigned ipointer = index;
    unsigned size;
    memcpy(&size, (char*)memptr + ipointer, sizeof(size));
    printf("reading %d bytes\n", size);
    ipointer += sizeof(size);
    memcpy(datapointer, (char*)memptr + ipointer, size);
    ipointer += size;
    return ipointer;
}
