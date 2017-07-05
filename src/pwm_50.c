#include "pwm_50.h"

int throttle = 0;
char flag = 0;

void *pwm_50(void *ptr){
	FILE *port_value;
	port_value = (FILE *) ptr;
	while(flag != 'q'){
		fprintf(port_value, "%d", 1);
		fflush(port_value);
		usleep(throttle);
		fprintf(port_value, "%d", 0);
		fflush(port_value);
		usleep(20000-throttle);
	}
}
