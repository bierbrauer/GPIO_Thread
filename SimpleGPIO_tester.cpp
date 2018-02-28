#include "SimpleGPIO_thread.h"
#include <stdio.h>



int main(int argc, char **argv){
	
	/*SimpleGPIO_thread * myGPIO= SimpleGPIO_thread::SimpleGPIO_threadMaker (23, 0, (unsigned int)2e04, (unsigned int)2e04, (unsigned int)20, 2) ;
	printf ("GPIO peri users = %d.\n", SimpleGPIO_thread::GPIOperi_users);
	SimpleGPIO_thread * myGPIO2= SimpleGPIO_thread::SimpleGPIO_threadMaker (22, 1, (float)25, (float)0.5, (float)8.0, 2) ;
	printf ("GPIO peri users = %d.\n", SimpleGPIO_thread::GPIOperi_users);
	if ((myGPIO == nullptr) || (myGPIO2 == nullptr)){
		printf ("SimpleGPIO_thread object was not created. Now exiting...\n");
		return 1;
	}
	myGPIO->DoTasks(5);
	myGPIO2->DoTasks(5);
	myGPIO->waitOnBusy (60);
	delete (myGPIO);
	printf ("GPIO peri users = %d.\n", SimpleGPIO_thread::GPIOperi_users);
	myGPIO2 ->setLevel (1, 0);
	delete (myGPIO2);
	printf ("GPIO peri users = %d.\n", SimpleGPIO_thread::GPIOperi_users);
	*/
	SimpleGPIO_thread *  myGPIO3= SimpleGPIO_thread::SimpleGPIO_threadMaker (23, 0, (unsigned int)480,(unsigned int)20, (unsigned int)50, 1) ;
	printf ("GPIO peri users = %d.\n", GPIOperi_users);
	if (myGPIO3 == nullptr){
		printf ("SimpleGPIO_thread object was not created the second time. Now exiting...\n");
		return 1;
	}
	float * endFuncArrayData = new float [128];
	myGPIO3->cosineDutyCycleArray (endFuncArrayData, 128, 64, 0.6, 0.4);
	myGPIO3->setUpEndFuncArray (endFuncArrayData, 128, 1);
	myGPIO3->setEndFunc (&pulsedThreadDutyCycleFromArrayEndFunc);
	myGPIO3->DoTasks(1280);
	myGPIO3->waitOnBusy (600);
	//myGPIO3 ->setLevel (1, 0);
	delete (myGPIO3);
	printf ("GPIO peri users = %d.\n", GPIOperi_users);
	
	return 0;
}


/*
 g++ -O3 -std=gnu++11 -Wall -lpulsedThread GPIOlowlevel.cpp SimpleGPIO_thread.cpp SimpleGPIO_tester.cpp -o Tester
*/