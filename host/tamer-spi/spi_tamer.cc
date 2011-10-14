// Implementation of SPI interface to ClockTamer

#include "spi_tamer.h"
#include <assert.h>
#include <string.h>
#ifdef __linux__
#  include <unistd.h> // for usleep()
#endif

#include <stdio.h> // For debug only

ClockTamerSpi::ClockTamerSpi(SpiInterfaceAbstract *spiInterface)
: mpSpiInterface(spiInterface)
, mUnansweredNum(0)
{
   assert(mpSpiInterface);
}

int ClockTamerSpi::send(const char *command, char *answerBuf, int answerBufLen)
{
   int commandLen = strlen(command);
   uint8_t data;
   int answerLen = 0;
  

   // There should be real data
   assert(command != NULL);
   assert(answerBuf != NULL);
   
   // Command should not include any CR and LF
   assert(strchr(command, 0x0D) == NULL);
   assert(strchr(command, 0x0A) == NULL);

   // Due to full-duplex nature of SPI we should be ready to receive at least
   // as much as we send.
   assert(answerBufLen > commandLen);
   
   // Asynchronous execution is not supported for now
   assert(mUnansweredNum == 0);

   for (int i=0; i<commandLen; i++)
   {
      mpSpiInterface->spiTransfer(data, command[i]);
      // TODO: receive data
      if (data != SPI_FILLER)
         fprintf(stderr, ">0x%X(%c) ", data, data<31?'?':data);
      assert(data == SPI_FILLER);
   }
   mpSpiInterface->spiTransfer(data, 0x0D);
   // TODO: receive data
   assert(data == SPI_FILLER);
   mpSpiInterface->spiTransfer(data, 0x0A);
   // TODO: receive data
   assert(data == SPI_FILLER);
   if (commandLen > 0)
   {
      // We should wait for answer is commmand is not empty
      mUnansweredNum++;
   }
   
   while (mUnansweredNum > 0)
   {
      // We need only last answer, so just ignore all previous data
//      answerLen = 0;
      
      mpSpiInterface->spiTransfer(data);
      if (data != SPI_FILLER && answerLen < answerBufLen)
      {
         fprintf(stderr, "0x%X(%c) ", data, data<31?'?':data);
         answerBuf[answerLen++] = data;
         if (data == 0x0A && answerLen > 1 && answerBuf[answerLen-2] == 0x0D)
         {
            // Answer received
            mUnansweredNum--;
            // Cut off CRLF from the end
            answerLen -= 2;
         }
      }
      
      // If we haven't received any data, just wait a little bit to prevent
      // CPU monopolizaton
      if (data == SPI_FILLER)
      {
#ifdef __linux__
         usleep(10);
#elif defined(WIN32)
         Sleep(1);
#else
         sleep(1);
#endif
      }
   }
   
   // Put \0 at the end of answer
   if (answerLen < answerBufLen)
   {
      answerBuf[answerLen] = 0;
   }

   return mUnansweredNum;
}
