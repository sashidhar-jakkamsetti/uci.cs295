#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


void report_and_exit(const char* msg) 
{
    perror(msg);
    exit(-1); /* EXIT_FAILURE */
}

void writetofile(const int fd, const void *datapointer, const int data_len)
{
    write(fd, &data_len, sizeof(int));
    write(fd, datapointer, data_len);
}

int readfromfile(const int fd, void *datapointer)
{
    int sizetoread;
    read(fd, &sizetoread, sizeof(int));
    read(fd, datapointer, sizetoread);
    return sizetoread;
}
