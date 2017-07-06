#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
//#include <unistd.h>

#include "gpio_init.h"
#include "pwm_50.h"
#include "angle.h"

#define THROTTLE_STEP 1

void *ESC_calib(void *ptr);
void *ESC_control(void *ptr);

int main(){
	//----------------------------------
	//Initiation of GPIO port as output
	//----------------------------------
	int gpio_port = 0;
	printf("*********************************************************\n");
	printf("\t\tWelcome to the leveling stick!\t\t*\n");
	printf("*********************************************************\n");

	while(1){
		printf("Choose RPi3 GPIO for ESC controller\n");
		printf("GPIO: ");
		scanf("%d", &gpio_port);
		if(gpio_export(gpio_port)){
			printf("Unable to export GPIO_GEN%d\n", gpio_port);
			exit(1);
		}
		else{
			printf("GPIO_GEN%d exported successfully\n", gpio_port);
			break;
		}
	}
	char direction[] = "out";
	printf("Length: %d\n", strlen(direction));
	if(gpio_direction(gpio_port, direction, strlen(direction))){
		printf("Unable to set GPIO_GEN%d as output\n", gpio_port);
		exit(1);
	}
	else
		printf("GPIO_GEN%d successfully set as output\n", gpio_port);
	FILE *port_value;
	if((port_value = gpio_value(gpio_port)) == NULL){
		printf("Unable to attach file pointer to GPIO_GEN%d\n", gpio_port);
		exit(1);
	}
	//--------------------------------------------
	//GPIO is ready to put square wave on output
	//--------------------------------------------
	int c;
	printf("*******************************************************\n");
	printf("Do you want to calibrate ESC?(y/n):");
	system("/bin/stty raw");
	while(1){
		c = getchar();
		if(c == 'y')
			break;
		if(c == 'n')
			break;
	}
	system("/bin/stty cooked");
	printf("*******************************************************\n");
	int iret1, iret2, iret3;
	pthread_t thread1, thread2, thread3;
	if(c == 'y'){
		iret1 = pthread_create(&thread1, NULL, pwm_50, (void *)port_value);
		if(iret1){
			printf("error pthread_create: pwm_50");
			exit(1);
		}
		iret2 = pthread_create(&thread2, NULL, ESC_calib, NULL);
		if(iret2){
			printf("error pthread_create: ESC_calib");
			exit(1);
		}
		pthread_join(thread1, NULL);
		pthread_join(thread2, NULL);
		flag = 0;
	}
	iret1 = pthread_create(&thread1, NULL, pwm_50, (void *)port_value);
	if(iret1){
		printf("error pthread_create: pwm_50");
	}
	iret2 = pthread_create(&thread2, NULL, ESC_control, NULL);
	if(iret2){
		printf("error pthread_create: ESC_control");
		exit(1);
	}
	iret3 = pthread_create(&thread3, NULL, angle, NULL);
	if(iret3){
		printf("error pthread_create: angle");
		exit(1);
	}
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);
	system("/bin/stty cooked");
}

void *ESC_calib(void *arg){
	int c = 0;
	printf("Disconnect the ESC power supply and then press 'n':\n");
	throttle = 2000;
	system("/bin/stty raw");
	while((c = getchar()) != 'n')
		;
	system("/bin/stty cooked");
	printf("Connect the ESC power supply, press 'n' after beep:\n");
	throttle = 1000;
	system("/bin/stty raw");
	while((c = getchar()) != 'n')
		;
	system("/bin/stty cooked");
	printf("You should now be ready to go!\n");
	flag = 'q';
}

void *ESC_control(void *arg){
	int c = 0, i = 0;
	throttle = 1000;
	printf("******************************************************\n");
	printf("Controll the ESC by either +/- for more/less speed\n");
	printf("Or set value directly by value in range 400-2000\n");
	printf("Press 'q' to quit\n");
	printf("******************************************************\n");
	system("/bin/stty raw");
	while((c = getchar()) != 'q'){
		switch(c){
			case '+':
				throttle = ((throttle + THROTTLE_STEP) < 2000) ? throttle + THROTTLE_STEP : throttle;
				break;
			case '-':
				throttle = ((throttle - THROTTLE_STEP) > 1000) ? throttle - THROTTLE_STEP : throttle;
				break;
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				system("/bin/stty cooked");
				i = i*10 + (c - '0');
				while(isdigit(c = getchar()))
					i = i*10 + (c - '0');
				if((i <= 2000) && (i >= 1000))
					throttle = i;
				i = 0;
				system("/bin/stty raw");
				break;
			default:
				break;
		}
	}
	system("/bin/stty cooked");
	flag = 'q';
}
