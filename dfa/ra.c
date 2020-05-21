/* REMOTE ATTESTATION MODULE */

#include "ra.h"
// #include "hmac.h"

#define MASTER_KEY_LEN     64
#define ONE_TIME_KEY_LEN   64
#define DIGEST_LEN         64


static uint8_t master_key[MASTER_KEY_LEN] = { 
                                    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                };

uint8_t one_time_key[ONE_TIME_KEY_LEN];

uint8_t digest[DIGEST_LEN];

// Key Derivation Function
// void hmac(output, key, key_len, input, input_len);


void hmac_init(const uint32_t main_start, const uint32_t main_end, const uint8_t *challenge, const int challenge_len)
{
    // hmac(one_time_key, master_key, MASTER_KEY_LEN, challenge, challenge_len);
    // hmac(one_time_key, one_time_key, ONE_TIME_KEY_LEN, &main_start, sizeof(main_start));
    // hmac(digest, one_time_key, ONE_TIME_KEY_LEN, &main_end, sizeof(main_end));
}

void hmac_update(const char* report_snip, const int report_snip_len)
{
    // hmac(digest, digest, DIGEST_LEN, (void *)report_snip, report_snip_len);
}

void hmac_quote(uint8_t *out, uint32_t *out_len)
{
    *((uint32_t*)out_len) = DIGEST_LEN;
    memcpy((void *) out, (void *) digest, DIGEST_LEN);
    memset((void *) one_time_key, 0, ONE_TIME_KEY_LEN);
    memset((void *) digest, 0, DIGEST_LEN);
}
