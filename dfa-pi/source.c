//Open Syringe Pump
//https://github.com/naroom/OpenSyringePump/blob/master/syringePump/syringePump.ino
//https://hackaday.io/project/1838-open-syringe-pump

#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <time.h>
#include "dfa_stub.h"


/* -- Constants -- */
#define SYRINGE_VOLUME_ML 10.0
#define SYRINGE_BARREL_LENGTH_MM 10.0

#define THREADED_ROD_PITCH 1
#define STEPS_PER_REVOLUTION 10.0
#define MICROSTEPS_PER_STEP 1.0

#define SPEED_MICROSECONDS_DELAY 4000

#define	false	0
#define	true	1

#define	boolean	_Bool

#define LED_OUT_PIN 0

#define InputFileName "input.data"
#define OutputFileName "digest.out"


/* -- Global variables -- */

// Input related variables
boolean inputStrReady = false;
int inputStrLen = 0;
char inputStr[10]; //input string storage

// Bolus size
float mLBolus; 

// Steps per ml
long ustepsPerML;
float mlPerStep;

/* -- Enums and constants -- */

//syringe movement direction
enum{PUSH,PULL}; 

enum{DEF,USE};
char report_snip[100];
/* -- Default Parameters -- */

float mLUsed = 0.0;

// Loop quit flag
int quit = 0;

// DFA monitor arguments
static uint8_t challenge[8] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
static uint32_t challenge_len;
static uint8_t quote_out[64];
static uint32_t quote_len;


void print_state()
{
	printf("\n\nEchoing data variables.... \n");
	printf("mlbolus: %f\n", mLBolus);
	printf("ustepsPerML: %ld\n", ustepsPerML);
	printf("mlPerStep: %f\n", mlPerStep);
	printf("input len: %d\n", inputStrLen);
}


void bolus(int direction)
{
	print_state();

	long steps = mLBolus * ustepsPerML;
	if(direction == PUSH)
    {
		printf("\nsetting the direction to PUSH out liquid....\n");
		mLUsed += mLBolus;
	}
	else if(direction == PULL)
    {
		printf("\nsetting the direction to PULL in liquid....\n");
		if((mLUsed-mLBolus) > 0)
        {
			mLUsed -= mLBolus;
		}
		else
        {
			mLUsed = 0;
		}
	}	

	// This delay value is changed accordingly to reflect the corresponding affect on the real syringe pump.
	float usDelay = (1.0 / ustepsPerML) * SPEED_MICROSECONDS_DELAY;
	
	for(long i=0; i < steps; i++)
    {
		printf("%s %6.3f ml (%s %6.3f ml in reality)\n", direction==PUSH ? "pushing":"pulling", 
				(float) (i + 1) * mlPerStep, direction==PUSH ? "pushed":"pulled", (float) (i + 1.0) * (1.0 / ustepsPerML));
        
		digitalWrite (LED_OUT_PIN, 1);
        delay(usDelay/2);

		digitalWrite (LED_OUT_PIN, 0);
		delay(usDelay/2);
	}
}


void process()
{
	if(inputStr[0] == '+')
    {
		bolus(PUSH);
	}
	else if(inputStr[0] == '-')
    {
		bolus(PULL);
	}
	else if(atoi(inputStr) != 0)
    {
		int uLbolus = atoi(inputStr);
		mLBolus = (float)uLbolus / 1000.0;

		print_state();
	}
	else if(strcmp(inputStr, "q") == 0)
    {
        quit = 1;
	}
    else
    {
        printf("Invalid command\n");
    }
	inputStrReady = false;
	inputStrLen = 0;
}


int getVal(char c)
{
	int rtVal = 0;
	if(c >= '0' && c <= '9')
	{
		rtVal = c - '0';
	}
	else
	{
		rtVal = c - 'a' + 10;
	}

	return rtVal;
}

void readInput()
{
	while (access(InputFileName, F_OK) == -1) 
	{
		sleep(2);
	} 

	FILE *fr = fopen (InputFileName, "r");
	char c = fgetc(fr);
	printf("\n======================================================\n");
	printf("\nEchoing the input....\n");
	// hex to char reader
	while (1)
	{
		char inchar = (char)(getVal(c) * 16 + getVal((char)fgetc(fr)));
		if( (int) inchar == 255) // Custom EOF
		{
			break;
		}
		inputStr[inputStrLen] = inchar;
		printf("%c",inputStr[inputStrLen] );
		inputStrLen++;
		fgetc(fr);
		c = fgetc(fr);
	}
	fclose(fr);
	remove(InputFileName);
	inputStr[inputStrLen] = '\0';
    inputStrReady = true;
}


void initialize()
{
	mLBolus = 0.5; // default bolus value

	// 10 steps per ml; that means 0.1 ml change per step.
	ustepsPerML = (MICROSTEPS_PER_STEP * STEPS_PER_REVOLUTION * SYRINGE_BARREL_LENGTH_MM) / (SYRINGE_VOLUME_ML * THREADED_ROD_PITCH );
	mlPerStep = (SYRINGE_VOLUME_ML * THREADED_ROD_PITCH ) / (MICROSTEPS_PER_STEP * STEPS_PER_REVOLUTION * SYRINGE_BARREL_LENGTH_MM);

	wiringPiSetup();
	pinMode (LED_OUT_PIN, OUTPUT);
	
	printf("\nStarting syringe pump.\n");
	comm_stub_init();
}

void terminate()
{
	printf("\nTerminating syringe pump.\n");
	comm_stub_end();
}

float gettime()
{
	struct timespec now;
	clock_gettime( CLOCK_MONOTONIC_RAW, &now );
	return (float)now.tv_sec + (float)now.tv_nsec / 1000000000;
}

void loop()
{
	readInput();
	if(inputStrReady)
    {
		float t = gettime();

		challenge_len = sizeof(challenge);
		dfa_init((uint32_t)&initialize, (uint32_t)&terminate, challenge, challenge_len);

		process();

		quote_len = sizeof(quote_out);
		dfa_quote(quote_out, &quote_len);

		t = gettime() - t;

		printf("\nelapsed time for the operation: %f", ((float)t));
		printf("\n");
		
		FILE* digest_file = fopen(OutputFileName, "w");
    	int digest_fd = fileno(digest_file);
    	write(digest_fd, quote_out, quote_len);
    	fclose(digest_file);
	}
}


int main(int argc, char* argv[]) 
{
	initialize();

	while(!quit) 
    {
        loop();
    }

	terminate();
}
