/* REMOTE ATTESTATION MODULE */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <memory.h>


void hmac_init(const uint32_t main_start, const uint32_t main_end, const uint8_t *challenge, const int challenge_len);

void hmac_update(const char* report_snip, const int report_snip_len);

void hmac_quote(uint8_t *out, int *out_len);
