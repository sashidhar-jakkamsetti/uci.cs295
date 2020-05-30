//Open Syringe Pump
//https://github.com/naroom/OpenSyringePump/blob/master/syringePump/syringePump.ino
//https://hackaday.io/project/1838-open-syringe-pump

#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include "dfa_stub.h"


/* -- Constants -- */
#define SYRINGE_VOLUME_ML 10.0
#define SYRINGE_BARREL_LENGTH_MM 10.0

#define THREADED_ROD_PITCH 1
#define STEPS_PER_REVOLUTION 10.0
#define MICROSTEPS_PER_STEP 1.0

#define SPEED_MICROSECONDS_DELAY 2000

#define	false	0
#define	true	1

#define	boolean	_Bool

#define LED_OUT_PIN 0

// 10 steps per ml; that means 0.1 ml change per step.
float mlPerStep = (SYRINGE_VOLUME_ML * THREADED_ROD_PITCH ) / (MICROSTEPS_PER_STEP * STEPS_PER_REVOLUTION * SYRINGE_BARREL_LENGTH_MM);
long ustepsPerML = (MICROSTEPS_PER_STEP * STEPS_PER_REVOLUTION * SYRINGE_BARREL_LENGTH_MM) / (SYRINGE_VOLUME_ML * THREADED_ROD_PITCH );

/* -- Enums and constants -- */
enum{PUSH,PULL}; //syringe movement direction

enum{DEF,USE};
char report_snip[100];
/* -- Default Parameters -- */
float mLBolus = 0; //default bolus size
float mLBigBolus = 1.000; //default large bolus size
float mLUsed = 0.0;

// Input related variables
char inputStr[10] = "";
boolean inputStrReady = false;
int inputStrLen = 0;

// Loop quit flag
int quit = 0;

// DFA monitor arguments
static uint8_t challenge[8] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
static uint32_t challenge_len;
static uint8_t quote_out[64];
static uint32_t quote_len;


void bolus(int direction)
{
	long steps = mLBolus * ustepsPerML;

		snprintf(report_snip, sizeof(report_snip), "%s%s%s%s%s", "File: ", __FILE__, "; Func: ", __func__, "; Var: ustepsPerML");
		dfa_primevariable_checker(2, (void *)&ustepsPerML, sizeof(ustepsPerML), report_snip, (int)strlen(report_snip), USE);

		snprintf(report_snip, sizeof(report_snip), "%s%s%s%s%s", "File: ", __FILE__, "; Func: ", __func__, "; Var: mLBolus");
		dfa_primevariable_checker(3, (void *)&mLBolus, sizeof(mLBolus), report_snip, (int)strlen(report_snip), USE);

		snprintf(report_snip, sizeof(report_snip), "%s%s%s%s%s", "File: ", __FILE__, "; Func: ", __func__, "; Var: steps");
		dfa_primevariable_checker(4, (void *)&steps, sizeof(steps), report_snip, (int)strlen(report_snip), DEF);
	if(direction == PUSH)
    {
		printf("setting the direction to PUSH out liquid....\n");
		mLUsed += mLBolus;

		snprintf(report_snip, sizeof(report_snip), "%s%s%s%s%s", "File: ", __FILE__, "; Func: ", __func__, "; Var: mLBolus");
		dfa_primevariable_checker(3, (void *)&mLBolus, sizeof(mLBolus), report_snip, (int)strlen(report_snip), USE);

		snprintf(report_snip, sizeof(report_snip), "%s%s%s%s%s", "File: ", __FILE__, "; Func: ", __func__, "; Var: mLUsed");
		dfa_primevariable_checker(5, (void *)&mLUsed, sizeof(mLUsed), report_snip, (int)strlen(report_snip), USE);

		snprintf(report_snip, sizeof(report_snip), "%s%s%s%s%s", "File: ", __FILE__, "; Func: ", __func__, "; Var: mLUsed");
		dfa_primevariable_checker(5, (void *)&mLUsed, sizeof(mLUsed), report_snip, (int)strlen(report_snip), DEF);
	}
	else if(direction == PULL)
    {
		printf("setting the direction to PULL in liquid....\n");
		if((mLUsed-mLBolus) > 0)
        {
			mLUsed -= mLBolus;

		snprintf(report_snip, sizeof(report_snip), "%s%s%s%s%s", "File: ", __FILE__, "; Func: ", __func__, "; Var: mLBolus");
		dfa_primevariable_checker(3, (void *)&mLBolus, sizeof(mLBolus), report_snip, (int)strlen(report_snip), USE);

		snprintf(report_snip, sizeof(report_snip), "%s%s%s%s%s", "File: ", __FILE__, "; Func: ", __func__, "; Var: mLUsed");
		dfa_primevariable_checker(5, (void *)&mLUsed, sizeof(mLUsed), report_snip, (int)strlen(report_snip), USE);

		snprintf(report_snip, sizeof(report_snip), "%s%s%s%s%s", "File: ", __FILE__, "; Func: ", __func__, "; Var: mLUsed");
		dfa_primevariable_checker(5, (void *)&mLUsed, sizeof(mLUsed), report_snip, (int)strlen(report_snip), DEF);
		}
		else
        {
			mLUsed = 0;

		snprintf(report_snip, sizeof(report_snip), "%s%s%s%s%s", "File: ", __FILE__, "; Func: ", __func__, "; Var: mLUsed");
		dfa_primevariable_checker(5, (void *)&mLUsed, sizeof(mLUsed), report_snip, (int)strlen(report_snip), DEF);
		}
	}	

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
	else if(atof(inputStr) != 0)
    {
		int uLbolus = atof(inputStr);
		mLBolus = (float)uLbolus / 1000.0;

		snprintf(report_snip, sizeof(report_snip), "%s%s%s%s%s", "File: ", __FILE__, "; Func: ", __func__, "; Var: mLBolus");
		dfa_primevariable_checker(3, (void *)&mLBolus, sizeof(mLBolus), report_snip, (int)strlen(report_snip), DEF);
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


void readInput()
{
    char inChar;
    while ((inChar = getchar()) != '\n') 
    {
        // vulnerable (buffer overflow) piece of code.
        inputStr[inputStrLen] = inChar;
        inputStrLen++;
    }
    inputStr[inputStrLen] = '\0';
    inputStrReady = true;
}

void initialize()
{
	printf("\nStarting syringe pump.\n");
	wiringPiSetup();
	pinMode (LED_OUT_PIN, OUTPUT) ;
	comm_stub_init();
}

void terminate()
{
	printf("\nTerminating syringe pump.\n");
	comm_stub_end();
}

void loop()
{
	readInput();
	if(inputStrReady)
    {
		challenge_len = sizeof(challenge);
		dfa_init((uint32_t)&initialize, (uint32_t)&terminate, challenge, challenge_len);

		process();

		quote_len = sizeof(quote_out);
		dfa_quote(quote_out, &quote_len);
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
