#ifndef ADC_H
#define ADC_H

#include <stdint.h>

void adcInit(void);
uint8_t adcConvert(uint8_t mux);

#endif
