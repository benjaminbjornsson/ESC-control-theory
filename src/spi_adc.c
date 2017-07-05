#include "spi_adc.h"

int main(){
	if(!bcm2835_init()){
		printf("bcm2835_init failed.\n");
		return 1;
	}
	
	if(!bcm2835_spi_begin()){
		printf("bcm2835_begin failed. \n");
		return 1;
	}
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536);
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
	
	uint8_t send_data = 0x23;
	
	uint8_t read_data;
	while(1){
		read_data = bcm2835_spi_transfer(send_data);
		printf("0x%02X\n", read_data);
	}
	
	printf("Sent to SPI: 0x%02X. Read back from SPI: 0x%02X.\n", send_data, read_data);
	if(send_data != read_data)
		printf("Do you hace the loopback from MOSI to MISO connected?\n");
	bcm2835_spi_end();
	bcm2835_close();
}
