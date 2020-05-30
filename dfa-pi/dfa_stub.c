/* Normal world API */

#include "util.h"
#include "dfa_stub.h"
#include "pishmem.h"

#define AccessPerms 0644

void comm_stub_init()
{
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
}

void comm_stub_end()
{
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

}

uint32_t dfa_init(const uint32_t main_start, const uint32_t main_end, const uint8_t *challenge, const int challenge_len)
{
	int func_id = 1;

	unsigned ipointer = 0;
	//----- READ / WRITE FROM SHARED MEMORY -----
	if (shared_memory1->some_flag == 0)
	{
		ipointer = writetoSmem(shared_memory1->some_data, ipointer, &func_id, sizeof(func_id));
		ipointer = writetoSmem(shared_memory1->some_data, ipointer, &main_start, sizeof(main_start));
		ipointer = writetoSmem(shared_memory1->some_data, ipointer, &main_end, sizeof(main_end));
		ipointer = writetoSmem(shared_memory1->some_data, ipointer, challenge, challenge_len);

		shared_memory1->some_flag = 1;
	}

	//----- SEMAPHORE RELEASE ACCESS -----
	if (!semaphore1_release_access())
		exit(EXIT_FAILURE);

	int error = 0;
	//----- SEMAPHORE GET ACCESS -----
	if (!semaphore1_get_access())
		exit(EXIT_FAILURE);
	else 
	{
		if (shared_memory1->some_flag == 1)
		{
			ipointer = 0;
			ipointer = readfromSmem(shared_memory1->some_data, ipointer, &error);

			shared_memory1->some_flag = 0;
		}	
	}
	return error;
}

uint32_t dfa_primevariable_checker(const int variable_id, const void *variable, const int variable_len, const char *report_snip, const int report_len, int event)
{
	int func_id = 2;

	unsigned ipointer = 0;
	//----- READ / WRITE FROM SHARED MEMORY -----
	if (shared_memory1->some_flag == 0)
	{
		ipointer = writetoSmem(shared_memory1->some_data, ipointer, &func_id, sizeof(func_id));
		ipointer = writetoSmem(shared_memory1->some_data, ipointer, &variable_id, sizeof(variable_id));
		ipointer = writetoSmem(shared_memory1->some_data, ipointer, variable, variable_len);
		ipointer = writetoSmem(shared_memory1->some_data, ipointer, report_snip, report_len);
		ipointer = writetoSmem(shared_memory1->some_data, ipointer, &event, sizeof(int));

		shared_memory1->some_flag = 1;
	}


  	//----- SEMAPHORE RELEASE ACCESS -----
	if (!semaphore1_release_access())
		exit(EXIT_FAILURE);

	int error = 0;
	//----- SEMAPHORE GET ACCESS -----
	if (!semaphore1_get_access())
		exit(EXIT_FAILURE);
	else 
	{
		if (shared_memory1->some_flag == 1)
		{
			ipointer = 0;
			ipointer = readfromSmem(shared_memory1->some_data, ipointer, &error);

			shared_memory1->some_flag = 0;
		}	
	}
	return error;
}

uint32_t dfa_quote(uint8_t *out, uint32_t *out_len)
{
	int func_id = 3;

	unsigned ipointer = 0;
	if (shared_memory1->some_flag == 0)
	{
		ipointer = writetoSmem(shared_memory1->some_data, ipointer, &func_id, sizeof(func_id));

		shared_memory1->some_flag = 1;
	}

  	//----- SEMAPHORE RELEASE ACCESS -----
	if (!semaphore1_release_access())
		exit(EXIT_FAILURE);

	int error = 0;
	//----- SEMAPHORE GET ACCESS -----
	if (!semaphore1_get_access())
		exit(EXIT_FAILURE);
	else 
	{
		if (shared_memory1->some_flag == 1)
		{
			ipointer = 0;
			*((unsigned*)out_len) = readlenfromSmem(shared_memory1->some_data, ipointer);
			ipointer = readfromSmem(shared_memory1->some_data, ipointer, out);
			ipointer = readfromSmem(shared_memory1->some_data, ipointer, &error);

			shared_memory1->some_flag = 0;
		}	
	}
	return error;
}
