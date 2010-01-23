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

#ifndef _USRP_SOURCE_H_
#define _USRP_SOURCE_H_

#include <usrp_standard.h>
#include "spi_interface.h"


class usrp_source : public SpiInterfaceAbstract {
public:
	enum usrp_side {
		side_A = 0,
		side_B = 1
	};

	enum usrp_rx_ant {
		ant_TXRX = 0,
		ant_RX2 = 1
	};

	usrp_source(float sample_rate);
	usrp_source(unsigned int decimation);
	~usrp_source();

	int open(usrp_side side, usrp_rx_ant antenna);
	int tune(double freq);
	void start();
	void stop();

	float sample_rate();

	int spiConfigure(int pinMOSI, int pinMISO, int pinSCK, int pinSS);
	void spiTransfer(uint8_t &dataIn, uint8_t dataOut=SPI_FILLER);

private:
	void calculate_decimation();

	usrp_standard_rx_sptr	m_u_rx;
	db_base_sptr		m_db_rx;
	usrp_side			m_side;

	float			m_sample_rate;
	float			m_desired_sample_rate;
	unsigned int		m_decimation;

	double			m_freq;

	/*
	 * This mutex protects access to the USRP and daughterboards but not
	 * necessarily to any fields in this class.
	 */
	pthread_mutex_t		m_u_mutex;

	struct UsrpSpiConfig
	{
		int pinMOSI;
		int pinMISO;
		int pinSCK;
		int pinSS;
	};
	UsrpSpiConfig mSpiConfig;

	static const unsigned int	CB_LEN		= (1 << 20);
	static const int		NCHAN		= 1;
	static const int		INITIAL_MUX	= -1;
	static const int		FUSB_BLOCK_SIZE	= 1024;
	static const int		FUSB_NBLOCKS	= 16 * 8;
	static const char *		FPGA_FILENAME() {
		return "std_2rxhb_2tx.rbf";
	}
};

#endif
