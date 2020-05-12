#include<stdio.h>
#include<stdlib.h>

#define	false	0
#define	true	1
#define SYRINGE_CAPACITY 5.0
#define	boolean	_Bool

/* -- Enums and constants -- */
enum{PUSH,PULL}; //syringe movement direction

// long ustepsPerML = (MICROSTEPS_PER_STEP * STEPS_PER_REVOLUTION * SYRINGE_BARREL_LENGTH_MM) / (SYRINGE_VOLUME_ML * THREADED_ROD_PITCH ); // check this

//input
char inputStr[80] = "";
boolean inputStrReady = false;
int inputStrLen = 0;

/* -- Default Parameters -- */
float mLBolus = 0.500; //default bolus size
float mLUsed = 0.0;
float mLRemaining = SYRINGE_CAPACITY;
int steps = 0;


void bolus(int direction){
	//Move stepper. Will not return until stepper is done moving.
  
	//change units to steps
	//long steps = (mLBolus * ustepsPerML);
	if(direction == PUSH){
		// steps = mLBolus * ustepsPerML;
        if(mLRemaining+mLBolus >= 0){
            mLUsed += mLBolus;
            mLRemaining -= mLBolus;
            printf("\n%f ml pushed out from the syringe.", mLBolus);
            printf("\n%f ml remaining in the syringe. ", mLRemaining);
        } else {
            printf("\nMedicine is over.");
			mLUsed = 0;
        }
        printf("\n******************************\n");
	}
	else if(direction == PULL){
		if((mLRemaining+mLBolus) <= SYRINGE_CAPACITY){
			mLUsed -= mLBolus;
            mLRemaining += mLBolus;
            printf("\n%f ml pulled in into the syringe.", mLBolus);
            printf("\n%f ml remaining in the syringe.", mLRemaining);
		}
		else{
            printf("\nSyringe is full.");
			mLUsed = 0;
		}
        printf("\n******************************\n");
	}	
}

void processInput(int currentActionIdx){
	//process inputs as they are read in
	if(inputStr[currentActionIdx] == '+'){
		bolus(PUSH);
	}
	else if(inputStr[currentActionIdx] == '-'){
		bolus(PULL);
	}
	else{
        printf("\nInvalid command!");
	}
}

void loop(){

	//dfa_init();
    for(int i=1; i<=steps; i++) {
        processInput(i);
    }
    //quote_len = sizeof(quote_out);
    //dfa_quote();
    inputStrReady = false;
}

int main(int argc, char* argv[]) { //args for CLI
	printf("\nStarting syringe pump\n");
    if(argc == 1) {
        printf("\nNo Input given. Try again.\n");
        return 0;
    }
    mLBolus = atof(argv[1]);
    printf("\nSize of Bolus: %f", mLBolus);
    if(mLBolus <= 0) { // check the float value
        printf("\nInvalid input!\n");
        return 0;
    }
    steps = argc-2; // number of actions being performed by the syringe pump
    inputStrReady = true;
    for(int i=1; i<argc-1; i++) {
        inputStr[i] = argv[i+1][0];
        printf("\nCurrent char: %c", inputStr[i]);
    }
	while(inputStrReady) {
		loop();
	}
}