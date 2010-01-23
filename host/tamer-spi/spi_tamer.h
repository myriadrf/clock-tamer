// SPI interface to ClockTamer

#ifndef _SPI_TAMER_H_
#define _SPI_TAMER_H_

#include "spi_interface.h"

class ClockTamerSpi {
public:
   
   enum {
      BUFFER_LEN = 40
   };

   ClockTamerSpi(SpiInterfaceAbstract *spiInterface);

   int send(const char *command, char *answerBuf, int answerBufLen);

protected:
   SpiInterfaceAbstract  *mpSpiInterface;
   int                    mUnansweredNum;
};


#endif
