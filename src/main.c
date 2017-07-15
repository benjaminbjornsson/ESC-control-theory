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
int init_bcm2835(void);

int main(){
	struct shared sharedValues;
	struct shared *sharedValuesPtr;
	sharedValuesPtr = &sharedValues;
	sharedValuesPtr->flag = 0;
	sharedValuesPtr->calibFlag = 0;
	sharedValuesPtr->throttle = 0;
	sharedValuesPtr->angle = 0;
	if(!init_bcm2835()){
		printf("Error: Unable to initialize bcm2835(), are you running as root?\n");
		exit(1);
	}
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
	if(c == 'y'){
		int iret1, iret2;
		pthread_t thread1, thread2;
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
		int iret3, iret4;
		pthread_t thread3, thread4;
		sharedValuesPtr->calibFlag = 1;
		iret3 = pthread_create(&thread3, NULL, angle_calibration, (void *) sharedValuesPtr);
		if(iret3){
			printf("error pthread_create: angle_calibration");
			exit(1);
		}
		iret4 = pthread_create(&thread4, NULL, angle, (void *) sharedValuesPtr);
		if(iret4){
			printf("error pthread_create: angle");
			exit(1);
		}
		pthread_join(thread3, NULL);
		pthread_join(thread4, NULL);
		while(sharedValuesPtr->flag != 'q')
			;
		sharedValuesPtr->flag = 0;
		sharedValuesPtr->calibFlag = 0;
	}
	else{
		FILE *fp;
		if((fp = fopen("values/values.txt", "r")) == NULL){
			printf("values/values.txt, no such file\n");
			exit(1);
		}
		float slope, theta_0, ADC_0;
		fscanf(fp, "#%f#%f#%f", &sharedValuesPtr->slope, &sharedValuesPtr->theta_0 , &sharedValuesPtr->ADC_0 );
		fclose(fp);
	}
	int iret5, iret6, iret7, iret8;
	pthread_t thread5, thread6, thread7, thread8;
	iret5 = pthread_create(&thread5, NULL, pwm_50, (void *) sharedValuesPtr);
	if(iret5){
		printf("error pthread_create: pwm_50");
	}
	iret6 = pthread_create(&thread6, NULL, angle, (void *) sharedValuesPtr);
	if(iret6){
		printf("error pthread_create: angle");
		exit(1);
	}
	iret7 = pthread_create(&thread7, NULL, PID, (void *) sharedValuesPtr);
	if(iret7){
		printf("error pthread_create: PID");
		exit(1);
	}
	iret8 = pthread_create(&thread8, NULL, ESC_control, (void *) sharedValuesPtr);
	if(iret8){
		printf("error pthread_create: ESC_control");
		exit(1);
	}
	pthread_join(thread5, NULL);
	pthread_join(thread6, NULL);
	pthread_join(thread7, NULL);
	pthread_join(thread8, NULL);
	system("/bin/stty cooked");
	bcm2835_close();
	return 0;
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

void *ESC_control(void *arg){
	struct shared *sharedPtr = (struct shared *)arg;
	printf("\n******************************************************\n");
	printf("Use '+', '-', 'p', 'i', 'd' and float values to controll\n");
	printf("K_p, K_i and K_d\n");
	printf("Press 'q' to quit\n");
	printf("******************************************************\n");
	system("/bin/stty raw");
	int c;
	char K_flag = 0;
	double num = 0;
	while((c = getchar()) != 'q'){
		switch(c){
			case 'p':
				K_flag = 'p';
				break;
			case 'i':
				K_flag = 'i';
				break;
			case 'd':
				K_flag = 'd';
				break;
			case '+':
				switch(K_flag){
					case 'p':
						sharedPtr->K_p += 0.01;
						break;
					case 'i':
						sharedPtr->K_i += 0.01;
						break;
					case 'd':
						sharedPtr->K_d += 0.01;
						break;
				}
				break;
			case '-':
				switch(K_flag){
					case 'p':
						sharedPtr->K_p -= 0.01;
						break;
					case 'i':
						sharedPtr->K_i -= 0.01;
						break;
					case 'd':
						sharedPtr->K_d -= 0.01;
						break;
				}
				break;
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				num = (c - '0');
				while(isdigit(c = getchar()))
					num = 10*num + (c - '0');
				if(c == '.'){
					int i;
					for(i = 1; isdigit(c = getchar()); i*=10)
						num = num + (c - '0')/(10.0*i);
				}
				switch(K_flag){
					case 'p':
						sharedPtr->K_p = num;
						break;
					case 'i':
						sharedPtr->K_i = num;
						break;
					case 'd':
						sharedPtr->K_d = num;
						break;
				}
				num = 0;
				break;
		}
		system("/bin/stty cooked");
		printf("\nK_p: %f\tK_i: %f\tK_d: %f\tangle: %f\tthrottle: %d\tlast flag: %c\n", sharedPtr->K_p ,sharedPtr->K_i, sharedPtr->K_d ,sharedPtr->angle, sharedPtr->throttle, K_flag);
		system("/bin/stty raw");
		usleep(500000);
	}
	system("/bin/stty cooked");
	sharedPtr->flag = 'q';
}

void *angle_calibration(void *ptr){
	struct shared *sharedValuesPtr = (struct shared *)ptr;
	FILE *fp;
	fp = fopen("values/values.txt", "w+");
	printf("\n*******************************************************\n");
	printf("ANGLE CALIBRATION\n");
	printf("Make sure the rig is in it's resting position and press 'n': ");
	int c = 0;
	system("/bin/stty raw");
	while(1){
		c = getchar();
		if(c == 'n')
			break;
		else if(c == 'q'){
			sharedValuesPtr->flag = 'q';
			system("/bin/stty cooked");
			return 0;
		}
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
		else if(c == 'q'){
			sharedValuesPtr->flag = 'q';
			system("/bin/stty cooked");
			return 0;
		}
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
	if((ADC_max-ADC_min) != 0)
		sharedValuesPtr->slope = (angle_max-angle_min)/(ADC_max-ADC_min);
	else{
		printf("Error: division by zero undefined\n");
	}
	sharedValuesPtr->theta_0 = angle_min;
	sharedValuesPtr->ADC_0 = ADC_min;
	sharedValuesPtr->flag = 'q';
	printf("#%f#%f#%f\n", sharedValuesPtr->slope, angle_min, ADC_min);
	fprintf(fp, "#%f#%f#%f", sharedValuesPtr->slope, sharedValuesPtr->theta_0, sharedValuesPtr->ADC_0);
	fflush(fp);
	fclose(fp);
}

void *user_interface_PID(void *arg){
	printf("*******************************************************\n");
	
	printf("*******************************************************\n");
}

int init_bcm2835(void){
	if(!bcm2835_init())
		return 0;
	return 1;
}
