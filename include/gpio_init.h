#include <stdio.h>
#include <unistd.h>
#include <string.h>

int gpio_export(int gpio_num);
int gpio_unexport(int gpio_num);
int gpio_direction(int gpio_num, char direction[], int strlen);
FILE *gpio_value(int gpio_num);
