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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#include <usrp/usrp_standard.h>
#include <usrp/usrp_subdev_spec.h>
#include <usrp/usrp_dbid.h>

#include "usrp_source.h"


usrp_source::usrp_source(float sample_rate) {

	m_desired_sample_rate = sample_rate;
	m_sample_rate = 0.0;
	m_decimation = 0;
	m_freq = 0.0;
	m_u_rx.reset();
	m_db_rx.reset();

	pthread_mutex_init(&m_u_mutex, 0);
}


usrp_source::usrp_source(unsigned int decimation) {

	m_freq = 0.0;
	m_sample_rate = 0.0;
	m_u_rx.reset();
	m_db_rx.reset();

	pthread_mutex_init(&m_u_mutex, 0);

	m_decimation = decimation & ~1;
	if(m_decimation < 4)
		m_decimation = 4;
	if(m_decimation > 256)
		m_decimation = 256;
}


usrp_source::~usrp_source() {

	stop();
	pthread_mutex_destroy(&m_u_mutex);
}


void usrp_source::stop() {

	pthread_mutex_lock(&m_u_mutex);
	if(m_db_rx)
		m_db_rx->set_enable(0);
	if(m_u_rx)
		m_u_rx->stop();
	pthread_mutex_unlock(&m_u_mutex);
}


void usrp_source::start() {

	pthread_mutex_lock(&m_u_mutex);
	if(m_db_rx)
		m_db_rx->set_enable(1);
	if(m_u_rx)
		m_u_rx->start();
	pthread_mutex_unlock(&m_u_mutex);
}


void usrp_source::calculate_decimation() {

	float decimation_f;

	decimation_f = (float)m_u_rx->fpga_master_clock_freq() /
	   m_desired_sample_rate;
	m_decimation = (unsigned int)round(decimation_f) & ~1;

	if(m_decimation < 4)
		m_decimation = 4;
	if(m_decimation > 256)
		m_decimation = 256;
}


float usrp_source::sample_rate() {

	return m_sample_rate;

}


int usrp_source::tune(double freq) {

	int r;
	usrp_tune_result tr;

	m_freq = freq;
	pthread_mutex_lock(&m_u_mutex);
	r = m_u_rx->tune(0, m_db_rx, freq, &tr);
	pthread_mutex_unlock(&m_u_mutex);

	return r;
}


/*
 * open() should be called before multiple threads access usrp_source.
 */
int usrp_source::open(usrp_side side, usrp_rx_ant antenna) {

	int do_set_decim = 0;
	m_side = side;
	usrp_subdev_spec ss(m_side, 0);
	printf("USRP side: %c\n", m_side==side_A?'A':'B');

	if(!m_u_rx) {
		if(!m_decimation) {
			do_set_decim = 1;
			m_decimation = 4;
		}
		if(!(m_u_rx = usrp_standard_rx::make(0, m_decimation,
		   NCHAN, INITIAL_MUX, usrp_standard_rx::FPGA_MODE_NORMAL,
		   FUSB_BLOCK_SIZE, FUSB_NBLOCKS, FPGA_FILENAME()))) {
			fprintf(stderr, "error: usrp_standard_rx::make: "
			   "failed!\n");
			return -1;
		}
		m_u_rx->stop();

		if(do_set_decim) {
			calculate_decimation();
			m_u_rx->set_decim_rate(m_decimation);
		}

		printf("FPGA clock: %ld\n", m_u_rx->fpga_master_clock_freq());
		printf("Decimation: %d\n", m_decimation);
		m_sample_rate = (double)m_u_rx->fpga_master_clock_freq() /
		   m_decimation;
	}
	if(!m_u_rx->is_valid(ss)) {
		fprintf(stderr, "error: invalid daughterboard\n");
		return -1;
	}
	if(!(m_db_rx = m_u_rx->selected_subdev(ss))) {
		fprintf(stderr, "error: no daughterboard\n");
		return -1;
	}

	m_u_rx->set_mux(m_u_rx->determine_rx_mux_value(ss));

	// no idea if this is a good gain for most boards
	m_db_rx->set_gain(m_db_rx->gain_min() +
	   0.70 * (m_db_rx->gain_max() - m_db_rx->gain_min()));
	printf("Antenna: %s\n", antenna==ant_RX2?"RX2":"TX/RX");
	m_db_rx->select_rx_antenna(antenna); // RFX RX-only antenna

	return 0;
}

int usrp_source::spiConfigure(int pinMOSI, int pinMISO, int pinSCK, int pinSS)
{
	mSpiConfig.pinMOSI = pinMOSI;
	mSpiConfig.pinMISO = pinMISO;
	mSpiConfig.pinSCK = pinSCK;
	mSpiConfig.pinSS = pinSS;
	// Configure pins direction
	m_u_rx->_write_oe(m_side,
	                  (1<<pinMOSI)|(0<<pinMISO)|(1<<pinSCK)|(1<<pinSS),
	                  (1<<pinMOSI)|(1<<pinMISO)|(1<<pinSCK)|(1<<pinSS));
	// Set SCK to high in idle mode and SS to high level
	m_u_rx->write_io(m_side, (1<<pinSCK)|(1<<pinSS), (1<<pinSCK)|(1<<pinSS));
	
	// Write 0xFF to kinda initialize SPI on slave. If we don't do this
	// we may get junk in first byte because changing OE register disturbs
	// ATMega SPI too much.
	uint8_t data;
	spiTransfer(data, SPI_FILLER);
	return 0;
}

void usrp_source::spiTransfer(uint8_t &dataIn, uint8_t dataOut)
{
	unsigned bit;

	// Set SS to low level (init SPI transfer)
	m_u_rx->write_io(m_side, (0<<mSpiConfig.pinSS), (1<<mSpiConfig.pinSS));

	for (bit = 0; bit < 8; bit++) {

		// Set MOSI
		m_u_rx->write_io(m_side, ((dataOut>>7)<<mSpiConfig.pinMOSI),
		                 (1<<mSpiConfig.pinMOSI));
		dataOut <<= 1;

		// SCK: high -> low
		m_u_rx->write_io(m_side, (0<<mSpiConfig.pinSCK), (1<<mSpiConfig.pinSCK));

		// Read MISO
		dataOut |= (m_u_rx->read_io(m_side)>>mSpiConfig.pinMISO) & 1;

		// SCK: low -> high
		m_u_rx->write_io(m_side, (1<<mSpiConfig.pinSCK), (1<<mSpiConfig.pinSCK));

	}

	// Set SS to high level (finish SPI transfer)
	m_u_rx->write_io(m_side, (1<<mSpiConfig.pinSS), (1<<mSpiConfig.pinSS));

	dataIn = dataOut;
}
