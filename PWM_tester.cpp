#include "PWM_thread.h"

#define pi 3.141592653589

float PWM_thread::PWMfreq =0;
int PWM_thread::PWMchans=0;
int PWM_thread::PWMrange =0;

int pwmRange = 4096; // PWM clock counts per output value, sets precision of output
float pwmFreq = 100; // desired frequency that PWM output value is updated, should be >= frequency requested for pulsed thread output
unsigned int arraySizeBase = 2048; // size of the array, must contain at least one period of output. A bigger array can put out a lower frequency 
float sin_frequency = 1; // requested sine wave frequency in Hz


int main(int argc, char **argv){
	
	float PWMfreq = 100;
	int PWMrange = 4096;
	int PWMchan = 0;
	int PWMmode = PWM_MARK_SPACE; //PWM_BALANCED; 

	unsigned int arraySizeBase = 2048; // size of the array, must contain at least one period of output. A bigger array can put out a lower frequency 
	
	
	// map peripherals for PWM controller
	int mapResult = PWM_thread::mapPeripherals ();
	if (mapResult){
		printf ("Could not map peripherals for PWM access with return code %d\n.", mapResult);
		return 1;
	}
	
	// set clock for PWM from constants
	bcm_PWM_Clockfreq = PWM_thread::setClock (PWMfreq, PWMrange);
	if (bcm_PWM_Clockfreq == -1){
		printf ("Could not set clock for PWM with frequency = %.3f and range = %d.\n", PWMfreq, PWMrange);
		return 1;
	}
	printf ("clock frequency = %.3f\n", bcm_PWM_Clockfreq);
	
	
	// calculate update frequency
	float updateFrequency = bcm_PWM_Clockfreq /pwmRange;
	printf ("Requested pwm update frequency=%.3f, Actual pwm update frequency is %.3f, PWM clock frequency is %.3f.\n", pwmFreq, updateFrequency, bcm_PWM_Clockfreq);	
	unsigned int periodSize = updateFrequency/sin_frequency;
	unsigned int arraySize = ((unsigned int)(arraySizeBase/periodSize)) * periodSize;
	
	// make data array at base size
	int * dataArray = new int [arraySizeBase];
	
	for (int i =0; i< arraySize; i++){
		dataArray [i] = (pwmRange/2 -  0.5*(pwmRange * cos (2 * pi * (i%periodSize)/periodSize)));
	}
	
	// make the thread
	
	PWM_thread * myPWM = PWM_thread::PWM_threadMaker (PWMchan, PWMmode, 1, dataArray, PWMrange, (unsigned int) 1000, (unsigned int) PWMrange, 1);
	if (myPWM == nullptr){
		printf ("thread maker failed to make a thread.\n");
		return 1;
	}
	//myPWM->setEnable (0, 1);
	myPWM->DoTasks (10);
	myPWM->waitOnBusy (30);
	/*
	INP_GPIO(GPIOperi ->addr,18);           // Set GPIO 18 to input to clear bits
	SET_GPIO_ALT(GPIOperi ->addr,18,5);     // Set GPIO 18 to Alt5 function PWM0
	unsigned int rangeRegisterOffset = PWM0_RNG;
	unsigned int dataRegisterOffset = PWM0_DAT;
	unsigned int modeBit = 0x80;
	unsigned int enableBit = 0x1;
	
	*(PWMperi ->addr  + rangeRegisterOffset) = PWMrange; // set range
	if (PWMmode ==PWM_MARK_SPACE){
		*(PWMperi ->addr + PWM_CTL) |= modeBit; // put PWM in MS Mode
	}else{
		*(PWMperi ->addr  + PWM_CTL) &= ~(modeBit);  // clear MS mode bit for balanced mode
	}
	// set initial PWM value first so we have something to put out
	*(PWMperi ->addr  + dataRegisterOffset) = 500;
	*(PWMperi ->addr  + PWM_CTL) |= enableBit;
	*/
	return 0; // 
}


	
	

/*

	
	
	
	
	// map peripherals for PWM controller
	int mapResult = PWM_thread::mapPeripherals ();
	if (mapResult){
		printf ("Could not map peripherals for PWM access with return code %d\n.", mapResult);
		return 1;
	}
	
	// set clock for PWM from constants
	float setFreq = PWM_thread::setClock (PWMfreq, PWMrange);
	if (setFreq == -1){
		printf ("Could not set clock for PWM with frequency = %.3f and range = %d.\n", PWMfreq, PWMrange);
		return 1;
	}
	printf ("clock frequency = %.3f\n", setFreq);
	// make some array data, a simple ramp
	int * arrayData = new int [PWMrange];
	for (int iData =0; iData < PWMrange; iData +=1){
		arrayData [iData] = iData;
	}
	printf ("first ArrayData = %d,%d,%d,%d,%d \n", arrayData [0], arrayData [1] , arrayData [2], arrayData [3] , arrayData [4] );
	
	// make the thread
	PWM_thread * myPWM = PWM_thread::PWM_threadMaker (PWMchan, PWMmode, 0, arrayData, PWMrange, (unsigned int) 1000, (unsigned int) PWMrange, 1);
	if (myPWM == nullptr){
		printf ("thread maker failed to make a thread.\n");
		return 1;
	}
	printf ("Wait on busy returned %d\n", myPWM->waitOnBusy(1));
	printf ("thread maker made a thread.\n");
	printf ("Thread channel = %d.\n", myPWM->PWM_chan);
	myPWM->setEnable (0, 1);
	myPWM->DoTasks (2);
	myPWM->waitOnBusy (60);
	myPWM->setEnable (1, 1);
	myPWM->DoTasks (2);
	myPWM->waitOnBusy (60);
	delete myPWM;
	
	delete arrayData;
}*/


// g++ -O3 -std=gnu++11 -Wall -lpulsedThread GPIOlowlevel.cpp PWM_thread.cpp PWM_tester.cpp -o PWMtester