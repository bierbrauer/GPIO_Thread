#include "HX711.h"

/* ************************************* Driver program for HX711 *************************************************
Does most everything you need to use to operate the HX711 amplifier with a load-cell  to weigh things
Use defaults of 22 and 27 for data pin and clock pin or start from command line with data pin and clock pin:
HX711_Scale 22 27

Compile like this:
g++ -O3 -std=gnu++11 -Wall -lpulsedThread GPIOlowlevel.cpp HX711.cpp HX711_runner.cpp -o HX711_Scale

Last modified:
2018/06/26 by jamie Boyd - added function to set sclaing from a single standard weight
2018/03/05 by Jamie Boyd - intial version for new pulsedThread verison of HX711.cpp 

* *****************Constants for pin defaults, scaling in grams per A/D units, and size of array for weight data **********************************/
const int kDATAPIN=22;
const int kCLOCKPIN = 27;
const float kSCALING = 7.15e-05;
const unsigned int kNUM_WEIGHTS=200;

/* ************************ Gets a line of input from stdin into a passed-in char buffer ************************************************************************************
returns false if there are too many characters on the line to fit into the passed in buffer
Last modified :
2016/06/01 by Jamie Boyd  - initial version */
 bool myGetline(char * line, int lenMax, int keepNewLine) {
	
	int len;
	int readVal;
	char readChar;
	for(len=0; len < lenMax; len +=1) {
		readVal = getchar(); // read next value into an int (need bigger than a char to account for EOF  -which we should never see anyways
		readChar = (char) readVal;
		line[len] =readChar;
		if ( readChar == '\n'){
			if (keepNewLine)
				len +=1;
			break;
		}
	}
	if (len == lenMax ){
		printf ("You entered too many characters on the line; the limit is %d\n", lenMax);
		return false;
	}else{
		line[len] = '\0';
		return true;
	}
}


int main(int argc, char **argv){
	int dataPin;
	int clockPin; 
	// parse input paramaters for dataPin and clockPin. If not present, use defaults
	if (argc == 3){
		dataPin = atoi (argv[1]);
		clockPin = atoi (argv[2]);
		if (((dataPin > 0) && (dataPin < 30)) && ((clockPin > 0) && (clockPin < 30))){
			printf ("Initializing HX711 from command line arguments dataPin= %d and clockPin = %d.\n", dataPin, clockPin);
		}else{
			printf ("Bad value for dataPin= %d or clockPin = %d, now exiting\n", dataPin, clockPin);
			return 1;
		}
	}else{
		dataPin = kDATAPIN;
		clockPin = kCLOCKPIN;
		printf ("Initializing HX711 with default values dataPin= %d and clockPin = %d.\n", dataPin, clockPin);
	}
	// make a floating point array to hold weights
	float * weightData = new float [kNUM_WEIGHTS];
	for (unsigned int ii =0; ii < kNUM_WEIGHTS; ii+=1){
		weightData [ii] = ii;
	}
	// make a HX711 thread object
	HX711 * scale = HX711::HX711_threadMaker (dataPin, clockPin, kSCALING, weightData, kNUM_WEIGHTS);
	if (scale == nullptr){
		printf("Could not create HX711_thread object.\n");
		return 1;
	}
	// make a temp buffer to hold a line of text to use with myGetLine
	int maxChars = 20;
	char * line = new char [maxChars];
	// some variables to handle menu selections
	signed char menuSelect;
	float newScaling;
	// Present menu and do selection, repeat until asked to quit
	for (;;){
		printf ("********** Enter a number from the menu below **********\n");
		printf ("-1:\tQuit the program.\n");
		printf ("0:\tTare the scale with average of 10 readings.\n");
		printf ("1:\tPrint current Tare value.\n");
		printf ("2:\tSet new scaling factor in grams per A/D unit\n");
		printf ("3:\tCalculate scaling from a standard weight\n");
		printf ("4:\tPrint current scaling factor.\n");
		printf ("5:\tWeigh something with a single reading\n");
		printf ("6:\tWeigh something with an average of 10 readings\n");
		printf ("7:\tStart a threaded read.\n"); 
		printf ("8:\tSet scale to low power mode\n");
		printf ("9:\tWake scale from low power mode\n");
		// scan the input into a string buffer
		if (myGetline(line, maxChars, 1)  == false)
			continue;
		// Get the menu selection, and check it
		sscanf (line, "%hhd\n", &menuSelect);
		if ((menuSelect < -1) || (menuSelect > 9)){
			printf ("You entered a selection, %hhd, outside the range of menu items (-1-8)\n", menuSelect);
			continue;
		} 
		switch (menuSelect){
			case -1:
				printf ("Quitting...\n");
				return 0;
				break;
			case 0:
				scale->tare (10, true);
			case 1:
				printf ("Tare Value is %.2f\n", scale->getTareValue());
				break;
			case 2:
				printf ("Enter new value for scaling factor in grams per A/D unit:");
				if (myGetline(line, maxChars, 1)  == false)
					break;
				sscanf (line, "%f\n", &newScaling);
				scale->setScaling (newScaling);
				break;
			case 3:
				printf ("Enter weight of standard in grams:");
				if (myGetline(line, maxChars, 1)  == false)
					break;
				sscanf (line, "%f\n", &newScaling);
				printf ("Calculated scaling  is %.5E grams per A/D unit\n",scale-> scalingFromStd (newScaling, 10));
				break;
			case 4:
				printf ("Scaling factor is %.5E grams per A/D unit\n",scale->getScaling());
				break;
			case 5:
				printf ("Measured Weight was %.3f grams.\n", scale->weigh (1,true));
				break;
			case 6:
				printf ("Measured Weight was %.3f grams.\n", scale->weigh ((unsigned int)10,true));
				break;
			case 7:
				scale->weighThreadStart (kNUM_WEIGHTS);
				struct timespec sleeper;
				sleeper.tv_sec = 0;
				sleeper.tv_nsec = 0.11E09;
				for (unsigned int nWeights=0; nWeights < 20;){
					nanosleep (&sleeper, NULL);
					nWeights =scale->weighThreadCheck ();
					printf ("Weight at %d = %.3f.\n", nWeights, weightData [nWeights-1]);
				}
				scale->weighThreadStop ();
				break;
			case 8:
				scale->turnOFF();
				break;
			case 9:
				scale->turnON ();
				break;
		}
	}
}