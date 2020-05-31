/* Normal world API */

#include "util.h"
#include "dfa_stub.h"

#define BackingFile "connector.mem"
#define ByteSize 512
#define AccessPerms 0644
#define SemaphoreName "connector.sem"

int fd;
sem_t* semptr;
void* memptr;


void comm_stub_init()
{
	fd = shm_open(BackingFile, O_RDWR | O_CREAT, AccessPerms);
    if (fd < 0) 
		report_and_exit("Can't open shared mem segment...");

    ftruncate(fd, ByteSize);
    memptr = mmap(NULL, ByteSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if ((caddr_t) -1  == memptr)
		report_and_exit("Can't get segment...");

    fprintf(stderr, "shared mem address: %p [0..%d]\n", memptr, ByteSize - 1);
    fprintf(stderr, "backing file:       %s\n", BackingFile );

	semptr = sem_open(SemaphoreName, O_CREAT, AccessPerms, 0);
	if (semptr == (void*) -1)
		report_and_exit("sem_open");
}

void comm_stub_end()
{
    munmap(memptr, ByteSize);
	close(fd);
    sem_close(semptr);
    shm_unlink(BackingFile);
}

uint32_t dfa_init(const uint32_t main_start, const uint32_t main_end, const uint8_t *challenge, const int challenge_len)
{
	int func_id = 1;

	unsigned ipointer = 0;
	ipointer = writetoSmem(memptr, ipointer, &func_id, sizeof(func_id));
	ipointer = writetoSmem(memptr, ipointer, &main_start, sizeof(main_start));
	ipointer = writetoSmem(memptr, ipointer, &main_end, sizeof(main_end));
	ipointer = writetoSmem(memptr, ipointer, challenge, challenge_len);

  	if (sem_post(semptr) < 0) 
		report_and_exit("sem_post");

	int error = 0;
	if (!sem_wait(semptr))
	{
		ipointer = 0;
		ipointer = readfromSmem(memptr, ipointer, &error);
	}
	return error;
}

uint32_t dfa_primevariable_checker(const int variable_id, const void *variable, const int variable_len, const char *report_snip, const int report_len, int event)
{
	int func_id = 2;

	unsigned ipointer = 0;
	ipointer = writetoSmem(memptr, ipointer, &func_id, sizeof(func_id));
	ipointer = writetoSmem(memptr, ipointer, &variable_id, sizeof(variable_id));
	ipointer = writetoSmem(memptr, ipointer, variable, variable_len);
	ipointer = writetoSmem(memptr, ipointer, report_snip, report_len);
	ipointer = writetoSmem(memptr, ipointer, &event, sizeof(int));

  	if (sem_post(semptr) < 0) 
		report_and_exit("sem_post");

	int error = 0;
	if (!sem_wait(semptr))
	{
		ipointer = 0;
		ipointer = readfromSmem(memptr, ipointer, &error);
	}
	return error;
}

uint32_t dfa_quote(uint8_t *out, uint32_t *out_len)
{
	int func_id = 3;

	unsigned ipointer = 0;
	ipointer = writetoSmem(memptr, ipointer, &func_id, sizeof(func_id));

  	if (sem_post(semptr) < 0) 
		report_and_exit("sem_post");

	int error = 0;
	if (!sem_wait(semptr))
	{
		ipointer = 0;
		*((unsigned*)out_len) = readlenfromSmem(memptr, ipointer);
		ipointer = readfromSmem(memptr, ipointer, out);
		ipointer = readfromSmem(memptr, ipointer, &error);
	}
	return error;
}
