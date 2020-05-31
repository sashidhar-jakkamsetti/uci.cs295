#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>

/* Normal world API */

void comm_stub_init();
void comm_stub_end();

uint32_t dfa_init(const uint32_t main_start, const uint32_t main_end, const uint8_t *challenge, const int challenge_len);

// mLBolus = 3 + 2;
// dfa_primevariable_checker(1, (void *)&mLBolus, sizeof(mLBolus), "f:bolus, v:mLBolus\n", 19, (int)DEF);
uint32_t dfa_primevariable_checker(const int variable_id, const void *variable, const int variable_len, const char *report_snip, const int report_len, int event);

uint32_t dfa_quote(uint8_t *out, uint32_t *out_len);
