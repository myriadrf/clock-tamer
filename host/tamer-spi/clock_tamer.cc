/*
 * Copyright (c) 2009, Joshua Lackey, Alexander Chemeris
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     *  Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *
 *     *  Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * clock_tamer
 *
 * Calculates the current offset using GSM FCCH bursts from local GSM
 * tower.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ncurses.h>

#include "usrp_source.h"
#include "spi_tamer.h"


static const unsigned int	AVG_COUNT	= 100;
static const unsigned int	AVG_THRESHOLD	= (2 * AVG_COUNT / 10);
static const float		OFFSET_MAX	= 100000;
static const unsigned int	NOTFOUND_MAX	= 10;


void usage(char *prog) {

	printf("usage: %s <-f frequency> [-s side] [-uh]\n", basename(prog));
	printf("\t-f\tfrequency\n");
	printf("\t-R\tdb side - \"A\" or \"B\", defaults to B\n");
	printf("\t-A\trx input - \"RX\" or \"RX2\", defaults to RX2\n");
	printf("\t-u\tdisplay continuously\n");
	printf("\t-h\thelp\n");
	exit(-1);
}


int main(int argc, char **argv) {

	int c;
	unsigned int decimation = 192;
	usrp_source::usrp_side subdev = usrp_source::side_A;
	usrp_source::usrp_rx_ant antena = usrp_source::ant_RX2;
	usrp_source *u = NULL;
   ClockTamerSpi *spi = NULL;


	while((c = getopt(argc, argv, "f:R:A:d:uh?")) != EOF) {
		switch(c) {
			case 'R':
			{
				if (strcmp(optarg, "A") == 0) {
					subdev = usrp_source::side_A;
				} else if (strcmp(optarg, "B") == 0) {
					subdev = usrp_source::side_B;
				} else {
					usage(argv[0]);
				}
				break;
			}
			case 'A':
			{
				if (strcmp(optarg, "RX2") == 0) {
					antena = usrp_source::ant_RX2;
				} else if (strcmp(optarg, "RX") == 0 || strcmp(optarg, "TX/RX") == 0 || strcmp(optarg, "RX/TX") == 0) {
					antena = usrp_source::ant_TXRX;
				} else {
					usage(argv[0]);
				}
				break;
			}
			case 'h':
			case '?':
			default:
				usage(argv[0]);
				break;
		}

	}

	u = new usrp_source(decimation);
	if(!u) {
		fprintf(stderr, "error: usrp_source\n");
		return -1;
	}
	if(u->open(subdev, antena) == -1) {
		fprintf(stderr, "error: usrp_source::open\n");
		return -1;
	}

	u->start();
	u->spiConfigure(10, 11, 9, 8);
   spi = new ClockTamerSpi(u);
   char command[1024];
   char answer[ClockTamerSpi::BUFFER_LEN+1];

//#define INF_LOOP
#ifdef INF_LOOP
   SpiInterfaceAbstract *iface = u;
   uint8_t data;
   while (1)
   {
      iface->spiTransfer(data, 0xF0);
      fprintf(stderr, "2: 0x%X\n", data);
   }
#else
   fprintf(stderr, "Your ClockTamer is listenning. Ready to command!\n");
   while (fgets(command, sizeof(command), stdin) != NULL) {
      int commandLen = strlen(command);
      while (commandLen > 0 && ( command[commandLen-1] == 0x0D || command[commandLen-1] == 0x0A ))
      {
         command[--commandLen] = 0;
      }
      if (commandLen == 0)
      {
         continue;
      }

      fprintf(stderr, "Sending command: \"%s\"\n", command);
      spi->send(command, answer, sizeof(answer));
      fprintf(stderr, "Received answer: \"%s\"\n", answer);
   };
#endif

   delete spi;
   delete u;

   return 0;
}
