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
#include "ra.h"

#define BackingFile "connector.mem"
#define ByteSize 512
#define AccessPerms 0644
#define SemaphoreName "connector.sem"
#define ReportFileName "report.out"

int fd;
sem_t* semptr;
void* memptr;
char* report;
int preport;

enum{DEF,USE};

struct kvp
{
    int key;
    void *value;
    int value_len;
};

static struct kvp primevariable_storage[100];
int nprimevariable;

static uint8_t quote_out[64];
static uint32_t quote_len;

unsigned ipointer;

void dfa_init()
{
    printf("in dfa_init()\n");

    report = (char *)malloc(1024);
    memset(report, 0, 1024);
    preport = 0;

    uint32_t main_start;
    uint32_t main_end;
    uint8_t *challenge;
    int challenge_len;
    
    ipointer = readfromSmem(memptr, ipointer, &main_start);
    ipointer = readfromSmem(memptr, ipointer, &main_end);
    challenge_len = readlenfromSmem(memptr, ipointer);
    challenge = (uint8_t *) malloc(challenge_len);
    ipointer = readfromSmem(memptr, ipointer, challenge);

    hmac_init(main_start, main_end, challenge, challenge_len);

    free(challenge);
}

void dfa_primevariable_checker()
{
    printf("in void dfa_primevariable_checker()\n");

    int variable_id;
    void *variable;
    int variable_len;
    char *report_snip;
    int report_snip_len;
    int event;

    ipointer = readfromSmem(memptr, ipointer, &variable_id);
    variable_len = readlenfromSmem(memptr, ipointer);
    variable = malloc(variable_len);
    ipointer = readfromSmem(memptr, ipointer, variable);
    report_snip_len = readlenfromSmem(memptr, ipointer);
    report_snip = (char *)malloc(report_snip_len);
    ipointer = readfromSmem(memptr, ipointer, report_snip);
    ipointer = readfromSmem(memptr, ipointer, &event);

    int found = 0;
    for (int i = 0; i < nprimevariable; i++)
    {
        if (primevariable_storage[i].key == variable_id)
        {
            found = 1;
            if (event == (int)DEF)
            {
                free(primevariable_storage[i].value);
                primevariable_storage[i].value = variable;
                primevariable_storage[i].value_len = variable_len;
            }
            else if (event == (int)USE)
            {
                if (variable_len != primevariable_storage[i].value_len 
                        || memcmp(primevariable_storage[i].value, variable, variable_len) != 0)
                {
                    printf("ERROR:in use\n");
                    memcpy(report + preport, report_snip, report_snip_len);
                    preport += report_snip_len;
                    hmac_update(report_snip, report_snip_len);
                }
            }
        }
    }

    if (found == 0 && event == (int)DEF)
    {
        primevariable_storage[nprimevariable].key = variable_id;
        primevariable_storage[nprimevariable].value = variable;
        primevariable_storage[nprimevariable].value_len = variable_len;
        nprimevariable += 1;
    }
}

void dfa_quote()
{
    printf("in dfa_quote()\n");

    uint8_t out[64];
    int out_len;

    hmac_quote(out, &out_len);
    ipointer = writetoSmem(memptr, ipointer, out, out_len);

    FILE* report_file = fopen(ReportFileName, "w");
    int report_fd = fileno(report_file);
    write(report_fd, report, preport);

    fclose(report_file);
    free(report);
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
                ipointer = 0;
                break;
            case 2:
                dfa_primevariable_checker();
                ipointer = 0;
                break;
            case 3:
                ipointer = 0;
                dfa_quote();
                break;
            
            default:
                printf("bad input func_id: %d\n", func_id);
                break;
        }

        int error = 0;
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

    nprimevariable = 0;
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