/*Assumptions made while writing the code*/
/*
1) Each operational mode will be assigned a value. The elec uC will send a value to comm uC and comm uC will switch to the operational mode corresponding to that value.
   For now, I have assigned 1 for idle, 2 for uplink, 3 for downlink.  
2) A different value will be sent to comm uC by elec uC in order to generate interrupt for collecting HM data from comm PCB. For now, I have assigned 4 for this purpose.
3) The HPS will be switched to uplink channel (connecting LNA and antenna) in default configuration.
4) For now, PORTA of the uC is assigned as output port for controlling the LNA(PA0), HPA(PA1 and PA2) and HPS(PA3 and PA4). Setting PA1 as high will turn on LNA, setting
   PA2 and PA3 will turn on HPA, PA4 as high and PA5 as low will be the default configuration for HPS, making PA4 as low and PA5 as high will switch the HPS to downlink.
*/

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

uint8_t opMode = 1;
uint8_t count = 0, i = 0;
uint8_t hmData[14];
uint8_t commData[5];
uint8_t uplinkData[12];

#include "usart.h"
#include "spi.h"
#include "adc.h"

int main()
{
	sei();
	usartInit();
	spiMasterInit();
	adcInit();
	PORTB &= ~(1<<PB0);
	DDRA=0x1F;
	PORTA=0x08;
	while(1)
	{
		switch(opMode)
		{
			/*nominal idle*/
			case 1: /*acknowledge elec uC*/
				break;
		
			/*nominal uplink*/
			case 2: /*acknowledge elec uC*/
				/*turn on LNA, put CC in receive mode*/
				PORTA=0x09;
				/*wait for some time*/
				/*collect data from CC*/
				SPDR=0xFF;
				_delay_ms(100);
				i=0;
				/*turn off LNA, put CC in standby mode*/
				PORTA=0x08;
				/*process the uplinked data*/
				opMode = 1;
				break;
			
			/*nominal downlink*/
			case 3: /*acknowledge elec uC*/
				/*switch HPS to downlink channel, turn on HPA, put CC in transmit mode*/
				PORTA=0x16;
				/*preprocess hm data for downlink*/
				/*send preprocessed data to CC*/
				for(int k=0;k<14;k++)
				{
					SPDR=hmData[k];
					_delay_ms(50);
				}
				/*wait for some time*/
				/*switch HPS back to uplink, turn off HPA, put CC in standby mode*/
				PORTA=0x08;
				opMode = 1;
				break;
		}
		
	}
	return 0;
}

ISR(USART0_RX_vect)
{
	if(opMode==3)
	{
		/*receiving HM data from elec for downlink*/
		hmData[count] = UDR0;
		count++;
		count%=14;
	}
	if(opMode!=3)
		opMode = UDR0; 
	if(opMode==4)
	{
		/*collection of HM data from comm PCB*/
		commData[0] = adcConvert(0x00);
		commData[1] = adcConvert(0x01);
		commData[2] = adcConvert(0x02);
		commData[3] = adcConvert(0x03);
		commData[4] = adcConvert(0x04);
		/*sending HM data of comm PCB to elec*/
		while(i<5)
		{
			UDR0=commData[i];
			i++;
		}
		i%=5;
		opMode = 1;
	}
}

ISR(USART0_TX_vect)
{
	
}

ISR(SPI_STC_vect)
{	
	if((opMode==2) & (i<12))
	{
		uplinkData[i]=SPDR;
		i++;
		SPDR=0xFF;
	}
}
