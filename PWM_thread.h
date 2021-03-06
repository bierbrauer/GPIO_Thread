#ifndef PWM_THREAD_H
#define PWM_THREAD_H

#include <pulsedThread.h>
#include <math.h>
#include "GPIOlowlevel.h"

/* ************************************** PWM Thread ***************************************************
Subclasses pulsedThread to output data from an array of values on the Raspberry Pi PWM (Pulse Width Modulator) peripheral
There should only ever be 1 PWM thread, and both channels of the pWM will be controlled on the same thread.
Could implement the whole singleton design pattern thing to ensure a single instance

****************************** Declare non-class functions used by pulsed thread ******************************/
void ptPWM_REG (void * taskData);
void ptPWM_FIFO_1 (void * taskDataP);
void ptPWM_FIFO_2 (void * taskDataP);
void ptPWM_FIFO_dual (void * taskDataP);
int ptPWM_Init (void * initData, void * &taskData);
void ptPWM_delTask (void * taskData);
int ptPWM_setFIFOCallback(void * modData, taskParams * theTask);
int ptPWM_addChannelCallback (void * modData, taskParams * theTask);
int ptPWM_setEnableCallback (void * modData, taskParams * theTask);
int ptPWM_reversePolarityCallback (void * modData, taskParams * theTask);
int ptPWM_setOffStateCallback (void * modData, taskParams * theTask);
int ptPWM_ArrayModCalback (void * modData, taskParams * theTask);

/* ******************** Custom Data Struct for pulsed Thread used by PWM ***************************
last modified:
2018/09/20 by Jamie Boyd - initial version */
typedef struct ptPWM_init_Struct{
	// using a FIFO or not is set on init
	int useFIFO;
	// addresses of functions for hiFunc are set at init
	void (*hiFuncREG)(void *);
	void (*hiFuncFIF1)(void *);
	void (*hiFuncFIF2)(void *);
	void (*hiFuncFIFdual)(void *);
}ptPWM_init_Struct, *ptPWM_init_StructPtr;

/* ******************** Custom Data Struct for pulsed Thread used by PWM ***************************
last modified:
2018/09/18 by Jamie Boyd - channels and outputs modifications
2018/09/12 by Jamie Boyd - added note that channel includes info on which pins to use
2018/08/07 byJamie Boyd - updating for pulsedThread subclass threading
2017/02/17 by Jamie Boyd - initial version */
typedef struct ptPWMStruct{
	int channels; // 1 for channel 1, 2 for channel 2
	int useFIFO; // set if using the FIFO, clear if using the data registers, If using FIFO, thread must be fast enough to keep up with PWM update
	// setings for each channel
	int chanFIFO; // alternating FIFO 
	int mode1; //MARK_SPACE for servos or BALANCED for analog
	int enable1; // 1 if channel is enabled, 0 if not enabled
	int polarity1; // 1 for reversed output polarity, 0 for normal, default is 0
	int offState1; // 0 for low level when PWM is not enabled, 1 for high level when PWM is enabled
	int audioOnly1; // set if output is directed to audio GPIO 40, not GPIO 18
	int mode2; //MARK_SPACE for servos or BALANCED for analog
	int enable2; // 1 if channel is enabled, 0 if not enabled
	int polarity2; // 1 for reversed output polarity, 0 for normal, default is 0
	int offState2; // 0 for low level when PWM is not enabled, 1 for high level when PWM is enabled
	int audioOnly2; // set if output is directed to audio GPIO 41, not GPIO 19
	// calculated register addresses, used for customDataMod functions
	volatile unsigned int * ctlRegister; // address of PWM control register
	volatile unsigned int * statusRegister; // address of status register
	volatile unsigned int * FIFOregister; // address of FIFO
	volatile unsigned int * dataRegister1; // address of register to write data to
	volatile unsigned int * dataRegister2; // address of register to write data to
	// addresses of functions for hiFunc
	void (*hiFuncREG)(void *);
	void (*hiFuncFIF1)(void *);
	void (*hiFuncFIF2)(void *);
	void (*hiFuncFIFdual)(void *);
	// data for outputting
	// data for outputting
	int * arrayData1; // array of PWM values for thread to cycle through, start to end, 0 to range-1
	unsigned int nData1; // number of points in data array
	unsigned int arrayPos1; // position in data array we are currently outputting
	unsigned int startPos1;
	unsigned int stopPos1;
	int * arrayData2; // array of PWM values for thread to cycle through, start to end, 0 to range-1
	unsigned int nData2; // number of points in data array
	unsigned int arrayPos2; // position in data array we are currently outputting
	unsigned int startPos2;
	unsigned int stopPos2;
} ptPWMStruct, *ptPWMStructPtr;


/* **************custom struct for callback configuring a PWM channel *****************************
Contains data for channel configurtation and pointer to data to output 
last modified:
2018/09/19 by Jamie Boyd - initial version */
typedef struct ptPWMchanAddStruct{
	int channel; // 1 for channel 1, or 2 for channel 2
	int audioOnly; // set to do outputs on audio pins only, not GPIO 18 or 19
	int mode; //MARK_SPACE for servos or BALANCED for analog
	int polarity; 
	int offState;
	int * arrayData; // array of PWM values for thread to cycle through, 0 to range, for channel
	unsigned int nData; // number of points in data array for channel 
}ptPWMchanAddStruct, *ptPWMchanAddStructPtr;

/* **************custom struct for callback changing PWM array settings *****************************
Used to modify which section of the array to currently use, or set current position in the array, or change array 
last modified:
2018/09/18 by Jamie Boyd - channels and outputs modifications
2018/08/08 by Jamie Boyd - initial verison */
typedef struct ptPWMArrayModStruct{
	int channel;			// 1 for channel 0, 2 for channel 1
	int modBits;			// bit-wise for which param to modify, 1 for startPos, 2 for stopPos, 4 for arrayPos, 8 for array data
	unsigned int startPos; // where to start in the array when out putting data
	unsigned int stopPos;	// where to end in the array
	unsigned int arrayPos;	// current position in array, as it is iterated through
	int * arrayData; //data for the array
	unsigned int nData; // size of data array
}ptPWMArrayModStruct, *ptPWMArrayModStructPtr;

/* **************custom struct for returning info about a PWM channel *****************************
Contains data for channel configurtation 
last modified:
2018/09/27 by Jamie Boyd - added chanel, you make the struct, fill in channel and pass the struct to getChannelInfo
2018/09/21 by Jamie Boyd - initial version */
typedef struct ptPWMchanInfoStruct{
	int theChannel;
	int audioOnly; // set to do outputs on audio pins only, not GPIO 18 or 19
	int PWMmode; //MARK_SPACE for servos or BALANCED for analog
	int enable; // 1 to start PWMing immediately, 0 to start in un-enabled state
	int polarity; 
	int offState;
}ptPWMchanInfoStruct, *ptPWMchanInfoStructPtr;

/* ********************************************* PWM_thread class *********************************************
last modified:
2018/09/30 by Jamie Boyd - better initialization options
2018/09/19 by Jamie Boyd - channels and outputs modifications
2018/08/08 by Jamie Boyd - initial verison */
class PWM_thread : public pulsedThread{
	public:
	/* constructors, one with unsigned ints for pulse delay and duration times in microseconds and number of pulses 
	PWM has no low func, so delay is 0, or dutyCycle is 1, and we don't know which hiFUnc to use til we have channels enabled*/
	PWM_thread (unsigned int durUsecs, unsigned int nPulses, void * initData, int (*initFunc)(void *, void * &), int accLevel , int &errCode) : pulsedThread (0, durUsecs, nPulses, initData, initFunc, nullptr, nullptr, accLevel, errCode) {
	};
	/* and the the other constructor with floats for frequency, duty cycle, and train duration */
	PWM_thread (float frequency, float trainDuration, void * initData, int (*initFunc)(void *, void * &), int accLevel, int &errCode) : pulsedThread (frequency, 1, trainDuration, initData, initFunc, nullptr, nullptr, accLevel, errCode){
	};
	virtual ~PWM_thread (void);
	// maps the GPIO, PWM, and PWMclock peripherals. Do this before doing anything else
	static int mapPeripherals ();
	// sets PWM clock for given frequency and range. Do this before making a PWM_thread, because thread makers need to kknow range and freq
	static float setClock (float PWMFreq, unsigned int PWMrange);
	// Static ThreadMakers make an initStruct and call a constructor with it, returning a pointer to a new PWM_thread
	static PWM_thread * PWM_threadMaker (float pwmFreq, unsigned int pwmRange, int useFIFO, unsigned int durUsecs, unsigned int nPulses, int accuracyLevel);
	static PWM_thread * PWM_threadMaker (float pwmFreq, unsigned int pwmRange, int useFIFO, float frequency, float trainDuration, int accuracyLevel);
	// configures one of the channels, 1 or 2, for output on the PWM. returns 0 for success, 1 for failure
	int addChannel (int channel, int audioOnly, int PWMmode, int polarity, int offState, int * arrayData, unsigned int nData);
	// set whether PWM is using FIFO, both channels are done the same way, either both are FIFO or neither is
	int setFIFO (int FIFOstate, int isLocking);
	// mod functions for enabling PWM output, setting polarity, and array modifications
	int setEnable (int enableState, int channel, int isLocking);
	int setPolarity (int polarityP, int channel, int isLocking);
	int setOffState (int offStateP, int channel, int isLocking);
	int setArraySubrange (unsigned int startPos, unsigned int stopPos, int channel, int isLocking);
	int setArrayPos (unsigned int arrayPos, int channel, int isLocking);
	int setNewArray (int * arrayData, unsigned int nData, int channel, int isLocking);
	float getPWMFreq (void);
	unsigned int getPWMRange (void);
	int getChannels (void);
	int getChannelInfo (ptPWMchanInfoStructPtr infoPtr);
	static unsigned int getStatusRegister (int verbose);
	static unsigned int getControlRegister (int verbose);
	// data members
	protected:
	float PWMfreq;
	unsigned int PWMrange;
	int useFIFO;
	int PWMchans; // bitwise pwm channels in use, 1 for channel 0, 2 for channel 1
	// config for channel 1
	int PWMmode1; // 0 for PWM_BALANCED, 1 for MARK_SPACE
	int polarity1; // 0 for normal polarity, 1 for reversed
	int offState1; // 0 for low when not enabled, 1 for high when enabled
	int enabled1; // 0 for not enabled, 1 for enabled
	int audioOnly1; // 0 for GPIO 18, 1 for default output over audio
	// config for channel 2
	int PWMmode2;
	int polarity2; // 0 for normal polarity, 1 for reversed
	int offState2; // 0 for low when not enabled, 1 for high when enabled
	int enabled2; // 0 for not enabled, 1 for enabled
	int audioOnly2; // 0 for GPIO 19, 1 for default output over audio
};

#endif
