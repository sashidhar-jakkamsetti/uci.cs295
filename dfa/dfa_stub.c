/* Normal world API */

#include "util.h"
#include "dfa_stub.h"

#define FileName "connector.mem"

struct flock lock;  // lock object on connector.mem
int fd;  // file descriptor to connector.mem


void comm_stub_init()
{
	lock.l_type = F_WRLCK;    /* read/write (exclusive versus shared) lock */
	lock.l_whence = SEEK_SET; /* base for seek offsets */
	lock.l_start = 0;         /* 1st byte in file */
	lock.l_len = 0;           /* 0 here means 'until EOF' */
	lock.l_pid = getpid();    /* process id */
	
	if ((fd = open(FileName, "w")) < 0) 
		report_and_exit("open failed...");
	
	lock.l_type = F_WRLCK;
	if (fcntl(fd, F_SETLK, &lock) < 0)
		report_and_exit("fcntl failed to get lock...");
}

void comm_stub_end()
{
	if (close(fd) < 0) 
		report_and_exit("close failed...");
}

uint32_t dfa_init(const uint32_t main_start, const uint32_t main_end, const uint8_t *challenge, const uint32_t challenge_len)
{
	int error;
	lock.l_type = F_WRLCK;
	if (fcntl(fd, F_SETLK, &lock) < 0)
		report_and_exit("fcntl failed to get lock...");
	else 
	{
		writetofile(fd, 1, sizeof(int));
		writetofile(fd, &main_start, sizeof(main_start)); 
		writetofile(fd, &main_end, sizeof(main_end)); 
		writetofile(fd, challenge, challenge_len); 
		fprintf(stderr, "Process %d has written to data file...\n", lock.l_pid);
	}

	lock.l_type = F_UNLCK;
	if (fcntl(fd, F_SETLK, &lock) < 0)
    	report_and_exit("explicit unlocking failed...");

	lock.l_type = F_WRLCK;
	if (fcntl(fd, F_SETLKW, &lock) < 0)
		report_and_exit("fcntl failed to get lock...");
	else 
	{
		readfromfile(fd, &error);
		fprintf(stderr, "Process %d has read from data file...\n", lock.l_pid);
	}
	return error;
}

uint32_t dfa_primevariable_checker(const int variable_id, const void *variable_address, const uint32_t variable_len, char event)
{
	int error;
	lock.l_type = F_WRLCK;
	if (fcntl(fd, F_SETLK, &lock) < 0)
		report_and_exit("fcntl failed to get lock...");
	else 
	{
		writetofile(fd, 2, sizeof(int));
		writetofile(fd, &variable_id, sizeof(variable_id)); 
		writetofile(fd, variable_address, variable_len); 
		writetofile(fd, event, sizeof(char)); 
		fprintf(stderr, "Process %d has written to data file...\n", lock.l_pid);
	}

	lock.l_type = F_UNLCK;
	if (fcntl(fd, F_SETLK, &lock) < 0)
    	report_and_exit("explicit unlocking failed...");

	lock.l_type = F_WRLCK;
	if (fcntl(fd, F_SETLKW, &lock) < 0)
		report_and_exit("fcntl failed to get lock...");
	else 
	{
		readfromfile(fd, &error);
		fprintf(stderr, "Process %d has read from data file...\n", lock.l_pid);
	}
	return error;
}

uint32_t dfa_quote(uint8_t *out, uint32_t *out_len)
{
	int error;
	lock.l_type = F_WRLCK;
	if (fcntl(fd, F_SETLK, &lock) < 0)
		report_and_exit("fcntl failed to get lock...");
	else 
	{
		writetofile(fd, 3, sizeof(int));
		fprintf(stderr, "Process %d has written to data file...\n", lock.l_pid);
	}

	lock.l_type = F_UNLCK;
	if (fcntl(fd, F_SETLK, &lock) < 0)
    	report_and_exit("explicit unlocking failed...");

	lock.l_type = F_WRLCK;
	if (fcntl(fd, F_SETLKW, &lock) < 0)
		report_and_exit("fcntl failed to get lock...");
	else 
	{
		*out_len = readfromfile(fd, out);
		readfromfile(fd, &error);
		fprintf(stderr, "Process %d has read from data file...\n", lock.l_pid);
	}
	return error;
}
