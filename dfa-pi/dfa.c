/* Secure world API */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "util.h"
#include "ra.h"
#include "pishmem.h"

#define AccessPerms 0644
#define ReportFileName "report.out"
#define OUT_LEN 64

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

unsigned ipointer;
int initialized = 0;

void dfa_init()
{
    report = (char *)malloc(1024);
    memset(report, 0, 1024);
    preport = 0;

    uint32_t main_start;
    uint32_t main_end;
    uint8_t *challenge;
    int challenge_len;
    
    ipointer = readfromSmem(shared_memory1->some_data, ipointer, &main_start);
    ipointer = readfromSmem(shared_memory1->some_data, ipointer, &main_end);
    challenge_len = readlenfromSmem(shared_memory1->some_data, ipointer);
    challenge = (uint8_t *) malloc(challenge_len);
    ipointer = readfromSmem(shared_memory1->some_data, ipointer, challenge);

    printf("in dfa_init(%d, %d)\n", main_start, main_end);

    char header[7] = "header:";
    char newline = '\n';

    memcpy(report + preport, header, sizeof(header));
    preport += sizeof(header);
    memcpy(report + preport, &main_start, sizeof(main_start));
    preport += sizeof(main_start);
    memcpy(report + preport, &main_end, sizeof(main_end));
    preport += sizeof(main_end);
    memcpy(report + preport, &newline, sizeof(newline));
    preport += sizeof(newline);

    hmac_init(main_start, main_end, challenge, challenge_len);
    initialized = 1;

    free(challenge);
}

void dfa_primevariable_checker()
{
    if (initialized == 0)
    {
        return;
    }

    int variable_id;
    void *variable;
    int variable_len;
    char *report_snip;
    int report_snip_len;
    int event;

    ipointer = readfromSmem(shared_memory1->some_data, ipointer, &variable_id);
    variable_len = readlenfromSmem(shared_memory1->some_data, ipointer);
    variable = malloc(variable_len);
    ipointer = readfromSmem(shared_memory1->some_data, ipointer, variable);
    report_snip_len = readlenfromSmem(shared_memory1->some_data, ipointer);
    report_snip = (char *)malloc(report_snip_len);
    ipointer = readfromSmem(shared_memory1->some_data, ipointer, report_snip);
    ipointer = readfromSmem(shared_memory1->some_data, ipointer, &event);

    printf("in void dfa_primevariable_checker(%d, %s)\n", variable_id, event == 0? "DEF" : "USE");

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
                    char newline = '\n';
                    memcpy(report + preport, &newline, sizeof(newline));
                    preport += sizeof(newline);
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
    if (initialized == 0)
    {
        return;
    }
    printf("in dfa_quote()\n");

    char out[OUT_LEN];
    int out_len;

    hmac_quote(out, &out_len);
    ipointer = writetoSmem(shared_memory1->some_data, ipointer, out, (int)out_len);

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
    if (semaphore1_get_access()) 
    {
        if (shared_memory1->some_flag == 1)
        {
            ipointer = readfromSmem(shared_memory1->some_data, ipointer, &func_id);
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
            ipointer = writetoSmem(shared_memory1->some_data, ipointer, &error, sizeof(error));
            
            //----- SEMAPHORE RELEASE ACCESS -----
            if (!semaphore1_release_access())
                exit(EXIT_FAILURE);
        }
    }
}


int main() 
{
    printf("\nStarting the Secure DFA Monitor.\n");

    printf("Creating shared memory with semaphore...\n");
	semaphore1_id = semget((key_t)12345, 1, AccessPerms | IPC_CREAT);		//<<<<< SET THE SEMPAHORE KEY (Must be unique and NOT the same as used on whatever other application is using the shared memory)   (Semaphore key, number of semaphores required, flags)
	//	Semaphore key
	//		Unique non zero integer (usually 32 bit).  Needs to avoid clashing with another other processes semaphores (you just have to pick a random value and hope - ftok() can help with this but it still doesn't guarantee to avoid colision)

	//Initialize the semaphore using the SETVAL command in a semctl call (required before it can be used)
	union semun sem_union_init;
	sem_union_init.val = 1;
	if (semctl(semaphore1_id, 0, SETVAL, sem_union_init) == -1)
	{
		fprintf(stderr, "Creating semaphore failed to initialize\n");
		exit(EXIT_FAILURE);
	}
	
	shared_memory1_id = shmget((key_t)1234, sizeof(struct shared_memory1_struct), AccessPerms | IPC_CREAT);		//<<<<< SET THE SHARED MEMORY KEY    (Shared memory key , Size in bytes, Permission flags)
	//	Shared memory key
	//		Unique non zero integer (usually 32 bit).  Needs to avoid clashing with another other processes shared memory (you just have to pick a random value and hope - ftok() can help with this but it still doesn't guarantee to avoid colision)

	if (shared_memory1_id == -1)
	{
		fprintf(stderr, "Shared memory shmget() failed\n");
		exit(EXIT_FAILURE);
	}

	//Make the shared memory accessible to the program
	shared_memory1_pointer = shmat(shared_memory1_id, (void *)0, 0);
	if (shared_memory1_pointer == (void *)-1)
	{
		fprintf(stderr, "Shared memory shmat() failed\n");
		exit(EXIT_FAILURE);
	}
	printf("Shared memory attached at %X\n", (int)shared_memory1_pointer);

	//Assign the shared_memory segment
	shared_memory1 = (struct shared_memory1_struct *)shared_memory1_pointer;

    nprimevariable = 0;
    while(1)
    {
        loop();
    }

    //Detach and delete
	if (shmdt(shared_memory1_pointer) == -1)
	{
		fprintf(stderr, "shmdt failed\n");
		//exit(EXIT_FAILURE);
	}
	if (shmctl(shared_memory1_id, IPC_RMID, 0) == -1)
	{
		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		//exit(EXIT_FAILURE);
	}
	//Delete the Semaphore
	//It's important not to unintentionally leave semaphores existing after program execution. It also may cause problems next time you run the program.
	union semun sem_union_delete;
	if (semctl(semaphore1_id, 0, IPC_RMID, sem_union_delete) == -1)
		fprintf(stderr, "Failed to delete semaphore\n");
    return 0;
}