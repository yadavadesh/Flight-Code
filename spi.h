#ifndef SPI_H
#define SPI_H

#include <stdint.h>

#define SS 0
#define SCK 1
#define MOSI 2
#define MISO 3

void spiMasterInit(void);

#endif
