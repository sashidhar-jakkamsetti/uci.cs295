/* Secure world API */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>
#include "util.h"


#define BackingFile "connector.mem"
#define ByteSize 512
#define AccessPerms 0644
#define SemaphoreName "connector.sem"

int fd;
sem_t* semptr;
void* memptr;

struct kvp
{
    int key;
    void *address;
    void *value;
};

static struct kvp primevariable_storage[100];

static uint32_t main_start;
static uint32_t main_end;
static uint8_t challenge[8] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
static uint32_t challenge_len;
static uint8_t quote_out[128];
static uint32_t quote_len;

unsigned ipointer;

void dfa_init()
{
    //ipointer = readfromSmem(memptr, ipointer, &main_start);
    printf("in dfa_init()\n");
}

void dfa_primevariable_checker()
{
    printf("in void dfa_primevariable_checker()\n");
}

void dfa_quote()
{
    printf("in dfa_quote()\n");
}

void loop()
{
    int func_id = 0;

    ipointer = 0;
    if (!sem_wait(semptr)) 
    {
        ipointer = readfromSmem(memptr, ipointer, &func_id);
        switch (func_id)
        {
            case 1:
                dfa_init();
                break;
            case 2:
                dfa_primevariable_checker();
                break;
            case 3:
                dfa_quote();
                break;
            
            default:
                printf("bad input func_id %d: \n", func_id);
                return;
        }

        int error = 0;
        ipointer = 0;
        ipointer = writetoSmem(memptr, ipointer, &error, sizeof(error));
        sem_post(semptr);
    }
}


int main() 
{
    printf("\nStarting the Secure DFA Monitor.\n");

    fd = shm_open(BackingFile, O_RDWR | O_CREAT, AccessPerms);
    if (fd < 0) 
        report_and_exit("Can't open shared mem segment...");

    ftruncate(fd, ByteSize);
    memptr = mmap(NULL, ByteSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if ((void *) -1  == memptr) 
        report_and_exit("Can't get segment...");

    fprintf(stderr, "shared mem address: %p [0..%d]\n", memptr, ByteSize - 1);
    fprintf(stderr, "backing file:       %s\n", BackingFile );

	semptr = sem_open(SemaphoreName, O_CREAT, AccessPerms, 0);
	if (semptr == (void*) -1) 
        report_and_exit("sem_open");

    while(1)
    {
        loop();
    }

    munmap(memptr, ByteSize);
	close(fd);
    sem_close(semptr);
    shm_unlink(BackingFile);
    return 0;
}