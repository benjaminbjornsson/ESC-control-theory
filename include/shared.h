#ifndef BB_ESC_SHARED
#define BB_ESC_SHARED
struct shared{
	volatile char flag;
	volatile int throttle;

	volatile unsigned angle;
	volatile double setpoint;

	volatile float K_p, K_i, K_d;
	
	volatile float slope;
	volatile float theta_0;
	volatile float ADC_0;
};
#endif
