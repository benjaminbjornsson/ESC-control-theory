#include "angle.h"

#define SAMPLE_RATE 1
#define MAX_SAMPLES 10

void *angle(void *ptr){
	struct shared *sharedValuesPtr = (struct shared *)ptr;
/*	if(!bcm2835_init()){
		printf("bcm2835_init failed, are you running as root?\n");
		sharedValuesPtr->flag = 'q';
		exit(1);
	}*/
	if(!bcm2835_spi_begin()){
		printf("bcm2835_begin failed, are you running as root?\n");
		sharedValuesPtr->flag = 'q';
		exit(1);
	}
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
	
		
	uint8_t dummy = 0;
	uint32_t len = 2;
	char tbuf[2], rbuf[2];
	int read_data = 0;
	
	rbuf[0] = 1;
	rbuf[1] = 2;

	int i = 1;

	double sample = 0;
	if(sharedValuesPtr->calibFlag == 1){
		while(sharedValuesPtr->flag != 'q'){
			bcm2835_spi_transfernb(tbuf, rbuf, len);
			read_data = (rbuf[0] & 0x1F) << 7;
			read_data = (read_data) | ((rbuf[1] >> 1) & 0x7F);
			sample += read_data;
			if(i == MAX_SAMPLES){
				sharedValuesPtr->angle = sample/MAX_SAMPLES;
				sample = 0;
				i = 1;
//				system("/bin/stty cooked");
//				printf("%f\n", sharedValuesPtr->angle);
//				system("/bin/stty raw");
			}
//			system("/bin/stty cooked");
//			printf("%d\t%d\n", rbuf[1], rbuf[0]);
//			system("/bin/stty raw");
			i++;
			usleep(SAMPLE_RATE);
		}
	}
	else{
		while(sharedValuesPtr->flag != 'q'){
			bcm2835_spi_transfernb(tbuf, rbuf, len);
			read_data = (rbuf[0] & 0x1F) << 7;
			read_data = (read_data) | ((rbuf[1] >> 1) & 0x7F);
			sample += read_data;
			if(i == MAX_SAMPLES){
				sharedValuesPtr->angle = sharedValuesPtr->theta_0 + (sharedValuesPtr->slope)*(sample/MAX_SAMPLES - sharedValuesPtr->ADC_0);
//				sharedValuesPtr->angle = sample/MAX_SAMPLES;
//				system("/bin/stty cooked");
//				printf("angle: %f\tthrottle: %d\n", sharedValuesPtr->angle, sharedValuesPtr->throttle);
//				system("/bin/stty raw");
				sample = 0;
				i = 1;
			}
			i++;
			usleep(SAMPLE_RATE);
		}
	}
	bcm2835_spi_end();
//	bcm2835_close();
	return 0;
}
