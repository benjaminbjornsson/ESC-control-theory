#include <stdio.h>

#include <bcm2835.h>

extern int throttle;
extern char flag;

void *pwm_50(void *ptr);
