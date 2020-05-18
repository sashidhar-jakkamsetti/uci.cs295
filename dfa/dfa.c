/* Secure world API */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "util.h"

#define FileName "connector.mem"

struct flock lock;  // lock object on connector.mem
int fd;  // file descriptor to connector.mem

typedef struct kv
{
    int key;
    void *value;
};

static struct kv[100];

static uint32_t main_start;
static uint32_t main_end;
static uint8_t challenge[8] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
static uint32_t challenge_len;
static uint8_t quote_out[128];
static uint32_t quote_len;

void dfa_init()
{

}

void dfa_primevariable_checker()
{

}

void dfa_quote()
{

}

void loop()
{
    int func_id = 0;

    while(1)
    {
        fcntl(fd, F_GETLK, &lock);
        if (lock.l_type == F_WRLCK)
            break;
    }

    lock.l_type = F_WRLCK;
    if (fcntl(fd, F_SETLKW, &lock) < 0)
		report_and_exit("fcntl failed to get lock...");
	else 
	{
		readfromfile(fd, &func_id);
		fprintf(stderr, "Process %d has read from data file...\n", lock.l_pid);
	}

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
            return;
    }
    writetofile(fd, 0, sizeof(int));
    
    lock.l_type = F_UNLCK;
	if (fcntl(fd, F_SETLK, &lock) < 0)
    	report_and_exit("explicit unlocking failed...");
}


int main() 
{
    lock.l_type = F_UNLCK;    /* unlock intially */
    lock.l_whence = SEEK_SET; /* base for seek offsets */
    lock.l_start = 0;         /* 1st byte in file */
    lock.l_len = 0;           /* 0 here means 'until EOF' */
    lock.l_pid = getpid();    /* process id */
    
    if ((fd = open(FileName, "w")) < 0)
        report_and_exit("open to read failed...");

    while(1)
    {
        loop()
    }

    close(fd);
    return 0;
}