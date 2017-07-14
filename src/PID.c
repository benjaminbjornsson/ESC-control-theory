#include "PID.h"
#include "shared.h"
#define THROTTLE_0 1550

void *PID(void * ptr){
	struct shared *sharedValuesPtr = (struct shared *) ptr;
	const unsigned int SLEEP_TIME = 1000;
	double dt = SLEEP_TIME/1000000;
	
	double integral = 0;
	double previousError = 0;
	sharedValuesPtr->K_p = 5;
	sharedValuesPtr->K_i = 0;
	sharedValuesPtr->K_d = 0;
	sharedValuesPtr->setpoint = 0;
	while(sharedValuesPtr->flag != 'q'){
		double error = sharedValuesPtr->setpoint - sharedValuesPtr->angle;
		integral += error*dt;
		double derivative = (error -previousError)/dt;
		
		previousError = error;
		
//		sharedValuesPtr->throttle = (THROTTLE_0 + sharedValuesPtr->K_p*error + sharedValuesPtr->K_i*integral + sharedValuesPtr->K_d*derivative);
		sharedValuesPtr->throttle = THROTTLE_0 + (sharedValuesPtr->K_p)*error + (sharedValuesPtr->K_i)*integral;
//		system("/bin/stty cooked");
//		printf("%d\n", sharedValuesPtr->throttle);
//		system("/bin/stty raw");
		usleep(1000);
	}
	printf("Now leaving function PID\n");
}
