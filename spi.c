#include "spi.h"
#include "avr_compiler.h"

void spiMasterInit(void)
{
	/*Set MOSI, SCK and SS as output*/
	DDRB = 0x07;
	PORTB |= (1<<PB0);
	/*Enable SPIE, SPI, master, clk rate fck/16*/
	SPCR = 0xD1;
}
