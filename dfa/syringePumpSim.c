//Open Syringe Pump
//https://github.com/naroom/OpenSyringePump/blob/master/syringePump/syringePump.ino
//https://hackaday.io/project/1838-open-syringe-pump

// Running through command line:
// Start the program  ------------------> ./syringePumpSim
// Type in size of the bolus (in uL) ---> 500
// Type in "+" to PUSH that size bolus -> +
// Type in "-" to PULL that size bolus -> -
// Type in "exit" to terminate ---------> q

#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <unistd.h>

/* -- Constants -- */
#define SYRINGE_VOLUME_ML 10.0
#define SYRINGE_BARREL_LENGTH_MM 10.0

#define THREADED_ROD_PITCH 1
#define STEPS_PER_REVOLUTION 10.0
#define MICROSTEPS_PER_STEP 1.0

#define SPEED_MICROSECONDS_DELAY 100 

#define	false	0
#define	true	1

#define	boolean	_Bool

float mlPerStep = (SYRINGE_VOLUME_ML * THREADED_ROD_PITCH ) / (MICROSTEPS_PER_STEP * STEPS_PER_REVOLUTION * SYRINGE_BARREL_LENGTH_MM);
long ustepsPerML = (MICROSTEPS_PER_STEP * STEPS_PER_REVOLUTION * SYRINGE_BARREL_LENGTH_MM) / (SYRINGE_VOLUME_ML * THREADED_ROD_PITCH );

/* -- Enums and constants -- */
enum{PUSH,PULL}; //syringe movement direction

/* -- Default Parameters -- */
float mLBolus = 0.500; //default bolus size
float mLBigBolus = 1.000; //default large bolus size
float mLUsed = 0.0;

// CommandLine Input
char inputStr[80] = "";
boolean inputStrReady = false;
int inputStrLen = 0;

// Loop quit flag
int quit = 0;

// DFA monitor arguments
// static uint8_t user_data[8] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
// static uint8_t quote_out[128];
// static uint32_t quote_len;

void bolus(int direction)
{
	long steps = mLBolus * ustepsPerML; // 100 steps per ml; that means 0.01 ml change per step.
	if(direction == PUSH)
    {
		printf("setting the direction to PUSH out liquid....\n");
		mLUsed += mLBolus;
	}
	else if(direction == PULL)
    {
		printf("setting the direction to PULL in liquid....\n");
		if((mLUsed-mLBolus) > 0)
        {
			mLUsed -= mLBolus;
		}
		else
        {
			mLUsed = 0;
		}
	}	

	float usDelay = SPEED_MICROSECONDS_DELAY; //can go down to 20 or 30

	for(long i=0; i < steps; i++)
    {
		printf("%s %6.3f ml (%s %6.3f ml in reality)\n", direction==PUSH ? "pushing":"pulling", 
				(float) (i + 1) * mlPerStep, direction==PUSH ? "pushed":"pulled", (float) (i + 1.0) * (1.0 / ustepsPerML));
        sleep(usDelay/100);
	}
}


void processInput()
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
        // vulnerable (buffer overflow) piece of code in arduino.
        inputStr[inputStrLen] = inChar;
        inputStrLen++;
    }
    inputStr[inputStrLen] = '\0';
    inputStrReady = true;
}


void loop()
{
	readInput();
	if(inputStrReady)
    {
		//dfa_init();
		processInput();
		//quote_len = sizeof(quote_out);
		//dfa_quote();
	}
}

// Main with command line arguments
int main(int argc, char* argv[]) 
{
    printf("\nStarting syringe pump\n");

    // infinite loop for reading and processing input
    while(!quit) 
    {
        loop();
    }
}
