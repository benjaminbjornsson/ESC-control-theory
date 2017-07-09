#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#include <bcm2835.h>

#include "pwm_50.h"
#include "angle.h"
#include "shared.h"
#include "PID.h"

#define THROTTLE_STEP 1

void *ESC_calib(void *ptr);
void *ESC_control(void *ptr);
void *angle_calibration(void *ptr);

int main(){
	struct shared sharedValues;
	struct shared *sharedValuesPtr;
	sharedValuesPtr = &sharedValues;
	sharedValuesPtr->flag = 0;
	sharedValuesPtr->throttle = 0;
	sharedValuesPtr->angle = 0;
	sharedValuesPtr = &sharedValues;
	printf("*********************************************************\n");
	printf("\t\tWelcome to the leveling stick!\t\t\n\n");
	printf("Do you want to calibrate ESC?(y/n):");
	system("/bin/stty raw");
	int c = 0;
	while(1){
		c = getchar();
		if(c == 'y')
			break;
		if(c == 'n')
			break;
	}
	system("/bin/stty cooked");
	printf("\n*******************************************************\n");
	int iret1, iret2, iret3, iret4;
	pthread_t thread1, thread2, thread3, thread4;
	if(c == 'y'){
		iret1 = pthread_create(&thread1, NULL, pwm_50, (void *) sharedValuesPtr);
		if(iret1){
			printf("error pthread_create: pwm_50");
			exit(1);
		}
		iret2 = pthread_create(&thread2, NULL, ESC_calib, (void *) sharedValuesPtr);
		if(iret2){
			printf("error pthread_create: ESC_calib");
			exit(1);
		}
		pthread_join(thread1, NULL);
		pthread_join(thread2, NULL);
		sharedValuesPtr->flag = 0;
	}
	printf("Do you want to calibrate angle sensor?(y/n):");
	system("/bin/stty raw");
	c = 0;
	while(1){
		c = getchar();
		if(c == 'y')
			break;
		if(c == 'n')
			break;
	}
	system("/bin/stty cooked");
	if(c == 'y'){
		iret1 = pthread_create(&thread1, NULL, angle_calibration, (void *) sharedValuesPtr);
		if(iret1){
			printf("error pthread_create: angle_calibration");
			exit(1);
		}
		iret2 = pthread_create(&thread2, NULL, angle, (void *) sharedValuesPtr);
		if(iret2){
			printf("error pthread_create: angle");
			exit(1);
		}
		pthread_join(thread1, NULL);
		pthread_join(thread2, NULL);
		while(sharedValuesPtr->flag != 'q')
			;
		sharedValuesPtr->flag = 0;
	}
	iret1 = pthread_create(&thread1, NULL, pwm_50, (void *) sharedValuesPtr);
	if(iret1){
		printf("error pthread_create: pwm_50");
	}
	iret2 = pthread_create(&thread3, NULL, angle, (void *) sharedValuesPtr);
	if(iret3){
		printf("error pthread_create: angle");
		exit(1);
	}
	iret3 = pthread_create(&thread4, NULL, PID, (void *) sharedValuesPtr);
	if(iret4){
		printf("error pthread_creat: PID");
		exit(1);
	}
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);
	system("/bin/stty cooked");
}

void *ESC_calib(void *arg){
	struct shared *sharedPtr = (struct shared *)arg;
	int c = 0;
	printf("Disconnect the ESC power supply and then press 'n':\n");
	sharedPtr->throttle = 2400;
	system("/bin/stty raw");
	while((c = getchar()) != 'n')
		;
	system("/bin/stty cooked");
	printf("Connect the ESC power supply, press 'n' after beep:\n");
	system("/bin/stty raw");
	while((c = getchar()) != 'n')
		;
	system("/bin/stty cooked");
	sharedPtr->throttle = 1200;
	printf("Press 'n' after beep:\n");
	system("/bin/stty raw");
	while((c = getchar()) != 'n')
		;
	printf("You should now be ready to go!\n");
	sharedPtr->flag = 'q';
}
/*
void *ESC_control(void *arg){
	struct shared *sharedPtr = (struct shared *)arg;
	int c = 0, i = 0;
	sharedPtr->throttle = 1200;
	printf("******************************************************\n");
	printf("Controll the ESC by either +/- for more/less speed\n");
	printf("Or set value directly by value in range 1200-2400\n");
	printf("Press 'q' to quit\n");
	printf("******************************************************\n");
	system("/bin/stty raw");
	while((c = getchar()) != 'q'){
		switch(c){
			case '+':
				sharedPtr->throttle = ((sharedPtr->throttle + THROTTLE_STEP) < 2400) ? sharedPtr->throttle + THROTTLE_STEP : sharedPtr->throttle;
				break;
			case '-':
				sharedPtr->throttle = ((sharedPtr->throttle - THROTTLE_STEP) > 1200) ? sharedPtr->throttle - THROTTLE_STEP : sharedPtr->throttle;
				break;
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				system("/bin/stty cooked");
				i = i*10 + (c - '0');
				while(isdigit(c = getchar()))
					i = i*10 + (c - '0');
				if((i <= 2400) && (i >= 1200))
					sharedPtr->throttle = i;
				i = 0;
				system("/bin/stty raw");
				break;
			default:
				break;
		}
	}
	system("/bin/stty cooked");
	sharedPtr->flag = 'q';
}*/

void *angle_calibration(void *ptr){
	struct shared *sharedValuesPtr = (struct shared *)ptr;
	printf("\n*******************************************************\n");
	printf("ANGLE CALIBRATION\n");
	printf("Make sure the rig is in it's resting position and press 'n': ");
	int c = 0;
	system("/bin/stty raw");
	while(1){
		c = getchar();
		if(c == 'n')
			break;
	}
	system("/bin/stty cooked");
	putchar('\n');
	float ADC_min = sharedValuesPtr->angle;
	printf("Raise the rig to it's max position and then press 'n': ");
	c = 0;
	system("/bin/stty raw");
	while(1){
		c = getchar();
		if(c == 'n')
			break;
	}
	system("/bin/stty cooked");
	putchar('\n');
	float ADC_max = sharedValuesPtr->angle;
	printf("Measure the angle of the rig at resting position(relative to the floor):");
	float angle_min;
	scanf("%f" , &angle_min);
	printf("Measure the angle of the rig at max position:");
	float angle_max;
	scanf("%f", &angle_max);
	printf("*******************************************************\n");
	sharedValuesPtr->slope = (angle_max-angle_min)/(ADC_max-ADC_min);
	sharedValuesPtr->theta_0 = angle_min;
	sharedValuesPtr->ADC_0 = ADC_min;
	sharedValuesPtr->flag = 'q';
}

void *user_interface_PID(void *arg){
	printf("*******************************************************\n");
	
	printf("*******************************************************\n");
}
