#include "angle.h"

void *angle(void *ptr){
	if(!bcm2835_init()){
		printf("bcm2835_init failed, are you running as root?\n");
		flag = 'q';
		exit(1);
	}
	if(!bcm2835_spi_begin()){
		printf("bcm2835_begin failed, are you running as root?\n");
		flag = 'q';
		exit(1);
	}
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_512);
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
	
	uint8_t dummy = 0;
	uint32_t len = 2;
	char tbuf[2], rbuf[2];
	int read_data = 0;
	while(flag != 'q'){
		system("/bin/stty cooked");
		bcm2835_spi_transfernb(tbuf, rbuf, len);
		read_data = (rbuf[0] & 0x1F) << 7;
		read_data = (read_data) | ((rbuf[1] >> 1) & 0x7F);
//		printf("throttle = %d\n", read_data);
		system("/bin/stty raw");
	}
	system("/bin/stty cooked");
	bcm2835_spi_end();
	bcm2835_close();
}
