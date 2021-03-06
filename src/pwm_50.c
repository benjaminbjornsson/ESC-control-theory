#include "pwm_50.h"

#define PIN RPI_GPIO_P1_12
#define PWM_CHANNEL 0
#define RANGE 24000

void *pwm_50(void *ptr){
	struct shared *sharedValuesPtr = (struct shared *)ptr;
/*	if(!bcm2835_init())
		return (void *)1;*/
	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_ALT5);
	bcm2835_pwm_set_clock(BCM2835_PWM_CLOCK_DIVIDER_16);
	bcm2835_pwm_set_mode(PWM_CHANNEL, 1, 1);
	bcm2835_pwm_set_range(PWM_CHANNEL, RANGE);
	
	while(sharedValuesPtr->flag != 'q'){
		bcm2835_pwm_set_data(PWM_CHANNEL,sharedValuesPtr->throttle);
		usleep(500);
	}
	sleep(1);
	bcm2835_pwm_set_data(PWM_CHANNEL,0);
//	bcm2835_close();
	return 0;
}
