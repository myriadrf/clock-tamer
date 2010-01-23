// Abstract SPI interface

#ifndef _SPI_INTERFACE_H_
#define _SPI_INTERFACE_H_

#include <stdint.h>

#define SPI_FILLER 0xFF

class SpiInterfaceAbstract {
public:
   virtual void spiTransfer(uint8_t &dataIn, uint8_t dataOut=SPI_FILLER) =0;
};


#endif
