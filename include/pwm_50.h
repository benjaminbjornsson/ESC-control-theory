#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/time.h>

extern int throttle;
extern char flag;
pthread_t thread1;

void *pwm_50(void *ptr);
