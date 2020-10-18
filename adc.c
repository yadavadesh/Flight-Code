#include "adc.h"
#include "avr_compiler.h"
#include "hmData.h"
#include <stdlib.h>

void adcInit(void)
{
	/*enabling ADC and setting prescaler to 16*/
	ADCSRA |= (1<<ADEN)|(1<<ADPS2);
}

uint8_t adcConvert(uint8_t mux)
{
	/*left adjust result*/
	ADMUX = (1<<ADLAR);
	ADMUX |= mux;
	/*start conversion*/
	ADCSRA |= (1<<ADSC);
	/*wait for conversion to finish*/
	while(!(ADCSRA & (1<<ADIF)));
	return ADCH;
}
