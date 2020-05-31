//Open Syringe Pump
//https://github.com/naroom/OpenSyringePump/blob/master/syringePump/syringePump.ino
//https://hackaday.io/project/1838-open-syringe-pump

#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <wiringPi.h>

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

#define InputFileName "input.hex"

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

/* -- Default Parameters -- */

float mLUsed = 0.0;

// Loop quit flag
int quit = 0;


void bolus(int direction)
{
	printf("\nmlbolus: %f\n", mLBolus);
	printf("ustepsPerML: %ld\n", ustepsPerML);
	printf("mlPerStep: %f\n", mlPerStep);
	printf("input len: %d\n", inputStrLen);
	long steps = mLBolus * ustepsPerML;
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

	float usDelay = (1.0 / ustepsPerML) * SPEED_MICROSECONDS_DELAY;
	
	for(long i=0; i < steps; i++)
    {
		printf("%s %6.3f ml (%s %6.3f ml in reality)\n", direction==PUSH ? "pushing":"pulling", 
				(float) (i + 1) * mlPerStep, direction==PUSH ? "pushed":"pulled", (float) (i + 1.0) * (1.0 / ustepsPerML));
		
		//digitalWrite (LED_OUT_PIN, 1);
        sleep(usDelay/2);

		//digitalWrite (LED_OUT_PIN, 0);
		sleep(usDelay/2);
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
	while ( access( InputFileName, F_OK ) == -1 ) 
	{
		sleep(0.5);
	} 

	FILE *fr = fopen (InputFileName, "rt");
	char c = fgetc(fr);
	printf("\n");
	while(c != EOF)
	{
		inputStr[inputStrLen] = (char)(getVal((char)c) * 16 + getVal((char)fgetc(fr)));
		printf("%c",inputStr[inputStrLen] );
		inputStrLen++;
		c = fgetc(fr);
		if (c == EOF)
		{
			break;
		}
		c = fgetc(fr);
	}
	fclose(fr);
	//remove(InputFileName);
	inputStr[inputStrLen] = '\0';
    inputStrReady = true;
}

void initialize()
{
	printf("\nStarting syringe pump.\n");

	mLBolus = 0.5; // default bolus value

	// 10 steps per ml; that means 0.1 ml change per step.
	ustepsPerML = (MICROSTEPS_PER_STEP * STEPS_PER_REVOLUTION * SYRINGE_BARREL_LENGTH_MM) / (SYRINGE_VOLUME_ML * THREADED_ROD_PITCH );
	mlPerStep = (SYRINGE_VOLUME_ML * THREADED_ROD_PITCH ) / (MICROSTEPS_PER_STEP * STEPS_PER_REVOLUTION * SYRINGE_BARREL_LENGTH_MM);

	//wiringPiSetup();
	//pinMode (LED_OUT_PIN, OUTPUT) ;
	//comm_stub_init();
}

void terminate()
{
	printf("\nTerminating syringe pump.\n");
	//comm_stub_end();
}

void loop()
{
	readInput();
	if(inputStrReady)
    {
		//challenge_len = sizeof(challenge);
		//dfa_init((uint32_t)&initialize, (uint32_t)&terminate, challenge, challenge_len);

		process();

		//quote_len = sizeof(quote_out);
		//dfa_quote(quote_out, &quote_len);
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
