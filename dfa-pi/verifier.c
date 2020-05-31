#include <time.h>
#include "ra.h"

#define OutputFileName "digest.out"
#define ReportFileName "report.out"

char header[7] = "header:";
char newline = '\n';

static uint8_t challenge[8] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
static uint8_t out[64];
static int out_len;


float gettime()
{
	struct timespec now;
	clock_gettime( CLOCK_MONOTONIC_RAW, &now );
	return (float)now.tv_sec + (float)now.tv_nsec / 1000000000;
}


int main()
{
    float t = gettime();
    FILE* report_file = fopen(ReportFileName, "r");
    char* line = NULL;
    int len = 0;
    int read = 0;

    uint32_t main_start, main_end;
    int nol = 0;
    while ((read = getline(&line, &len, report_file)) != -1)
    {
        nol++;
        if(nol == 1 && read >= sizeof(header) && memcmp(header, line, sizeof(header)) == 0)
        {
            memcpy(&main_start, line + sizeof(header), sizeof(main_start));
            memcpy(&main_end, line + sizeof(header) + sizeof(main_start), sizeof(main_end));
            hmac_init(main_start, main_end, challenge, sizeof(challenge));
        }
        else if(nol > 1 && read > 0)
        {
            t = gettime() - t;
            printf("MESSED UP!\n");
            printf("\nelapsed time for the operation: %f", ((float)t));
		    printf("\n");
            return 0;
        }
    }
    hmac_quote(out, &out_len);
    fclose(report_file);

    FILE* digest_file = fopen(OutputFileName, "r");
    uint8_t ref[64];
    if(fgets(ref, sizeof(ref), digest_file) != NULL && memcmp(out, ref, sizeof(ref)) == 0)
    {
        t = gettime() - t;
        printf("All good!\n");
        printf("\nelapsed time for the operation: %f", ((float)t));
		printf("\n");
        return 0;
    }

    t = gettime() - t;
    printf("MESSED UP!\n");
    printf("\nelapsed time for the operation: %f", ((float)t));
	printf("\n");
    return 0;
}