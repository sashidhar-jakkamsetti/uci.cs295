#include <sys/shm.h>
#include <sys/sem.h>

#define SHMEME_SIZE 256

//----- SEMAPHORE -----
//On linux systems this union is probably already defined in the included sys/sem.h, but if not use this default basic definition:
union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

//FUNCTIONS:
static int semaphore1_get_access(void);
static int semaphore1_release_access(void);
//VARIABLES:
static int semaphore1_id;

//----- SHARED MEMORY -----
struct shared_memory1_struct {
	int some_flag;
	char some_data[SHMEME_SIZE];
};

void *shared_memory1_pointer = (void *)0;
//VARIABLES:
struct shared_memory1_struct *shared_memory1;
int shared_memory1_id;

//***********************************************************
//***********************************************************
//********** WAIT IF NECESSARY THEN LOCK SEMAPHORE **********
//***********************************************************
//***********************************************************
//Wait if necessary and then change the semaphore by –1. This is the "wait" operation
static int semaphore1_get_access(void)
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = -1; /* P() */
	sem_b.sem_flg = SEM_UNDO;
	if (semop(semaphore1_id, &sem_b, 1) == -1)		//Wait until free
	{
		fprintf(stderr, "semaphore1_get_access failed\n");
		return(0);
	}
	return(1);
}

//***************************************
//***************************************
//********** RELEASE SEMAPHORE **********
//***************************************
//***************************************
//Setting the semaphore back to available.  This is the "release" operation.

static int semaphore1_release_access(void)
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = 1; /* V() */
	sem_b.sem_flg = SEM_UNDO;
	if (semop(semaphore1_id, &sem_b, 1) == -1)
	{
		fprintf(stderr, "semaphore1_release_access failed\n");
		return(0);
	}
	return(1);
}