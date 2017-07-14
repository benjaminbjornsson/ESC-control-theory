#ifndef BB_ESC_SHARED
#define BB_ESC_SHARED
struct shared{
	volatile char flag;
	volatile char calibFlag;
	volatile int throttle;

//	volatile float angle;
	volatile double angle;
	volatile double setpoint;

	volatile float K_p;
	volatile float K_i;
	volatile float K_d;
	
	volatile float slope;
	volatile float theta_0;
	volatile float ADC_0;
};
#endif
