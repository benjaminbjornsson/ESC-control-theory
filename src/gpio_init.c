#include "gpio_init.h"

#define MAX_ATTEMPT 100
#define MAX_TIME_ATTEMPT 5000

void port_num2str(int gpio_num, char *port_num){
	port_num[1] = gpio_num%10 + '0';
	gpio_num = gpio_num/10;
	port_num[0] = gpio_num%10 + '0';
	port_num[2] = '\0';
}

/*gpio_export tries to export GPIO_GEN#gpio_num
gpio_export returns 0 on success and -1 otherwise
*/
int gpio_export(int gpio_num){
	FILE *port_export;
	char port_num[3];
	port_num2str(gpio_num, port_num);
	int counter = 0;
	while(1){
		port_export = fopen("/sys/class/gpio/export", "w");
		if(port_export){
			if(fprintf(port_export, "%s", port_num) != 2){
				fclose(port_export);
				return -1;
			}
			fflush(port_export);
			fclose(port_export);
			return 0;
		}
		if(counter++ < MAX_ATTEMPT)
			usleep(MAX_TIME_ATTEMPT);
		else
			return -1;
	}
}
/*int gpio_export(int gpio_num) tries to unexport GPIO_GEN#gpio_num
returnvalues: 0 on success and -1 otherwise
*/
int gpio_unexport(int gpio_num){
	FILE *port_unexport;
	char port_num[3];
	port_num2str(gpio_num, port_num);
	int counter = 0;
	while(1){
		port_unexport = fopen("/sys/class/gpio/unexport", "w");
		if(port_unexport){
			if(fprintf(port_unexport, "%s", port_num) != 2){
				fclose(port_unexport);
				return -1;
			}
			fflush(port_unexport);
			fclose(port_unexport);
			return 0;
		}
		if(counter++ < MAX_ATTEMPT)
			usleep(MAX_TIME_ATTEMPT);
		else
			return -1;
	}
}

int gpio_direction(int gpio_num, char direction[], int strlen){
	FILE *port_direction;
	char port_num[3], full_dir[50] = "/sys/class/gpio/gpio";
	port_num2str(gpio_num, port_num);
	strcat(full_dir, port_num);
	strcat(full_dir, "/direction");
	int counter = 0;
	while(1){
		port_direction = fopen(full_dir, "w");
		if(port_direction){
			if(fprintf(port_direction, "%s", direction) != strlen){
				fclose(port_direction);
				return -1;
			}
			fflush(port_direction);
			fclose(port_direction);
			return 0;
		}
		if(counter++ < MAX_ATTEMPT)
			usleep(MAX_TIME_ATTEMPT);
		else
			return -1;
		
	}
}

FILE *gpio_value(int gpio_num){
	FILE *port_value;
	char port_num[3], full_dir[50] = "/sys/class/gpio/gpio";
	port_num2str(gpio_num, port_num);
	strcat(full_dir, port_num);
	strcat(full_dir, "/value");
	int counter = 0;
	while(1){
		port_value = fopen(full_dir, "w");
		if(port_value){
			if(fprintf(port_value, "%s", port_num) != 2){
				return NULL;
			}
			fflush(port_value);
			return port_value;
		}
		if(counter++ < MAX_ATTEMPT)
			usleep(MAX_TIME_ATTEMPT);
		else
			return NULL;
	}
}
