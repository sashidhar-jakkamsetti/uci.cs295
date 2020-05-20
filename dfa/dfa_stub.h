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

uint32_t dfa_init(const uint32_t main_start, const uint32_t main_end, const uint8_t *user_data, const uint32_t user_data_len);

// mLBolus = 3 + 2;
// dfa_primevariable_checker(1, (void)&mLBolus, sizeof(mLBolus), (char)DEF);
uint32_t dfa_primevariable_checker(const int variable_id, const void *variable_address, const uint32_t variable_len, char event);

uint32_t dfa_quote(uint8_t *out, uint32_t *out_len);
