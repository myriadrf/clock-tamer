/*
        ClockTamer - a software for configurable reference clock
                  Copyright (C) 2009, Fairwaves
          by Sergey Kostanbaev <Sergey.Kostanbaev@fairwaves.ru>
*/

/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _TAMER_CONFIG_H_
#define _TAMER_CONFIG_H_


#ifndef TAMER_VER
#error You should define TAMER_VER (10 stads for 1.0, 11 - 1.1, and 12 - 1.2, 121 - 1.21)
#endif

#if !defined(TAMER_1515_20) && \
    !defined(TAMER_2080_20) && \
    !defined(TAMER_2080_10) && \
    !defined(TAMER_2080_VCTXCO_26_GPS) && \
    !defined(TAMER_1650_26_GPS)

# if (TAMER_VER == 12 || TAMER_VER == 121 || TAMER_VER == 122 || TAMER_VER == 123)
#  define TAMER_2080_20
# elif TAMER_VER == 11
#  define TAMER_1515_20
# else
#  error You should define TAMER_1515_20, TAMER_2080_20, TAMER_2080_10, TAMER_2080_VCTXCO_26_GPS, TAMER_1650_26_GPS according to you model
# endif
#endif


#ifdef          PINS_ALL
#define DEF_OUT_MASK_LMK        (0xff)
#elif defined   PINS_TWO
#define DEF_OUT_MASK_LMK        ((1 << 1) | (1 << 6))
#else
#define DEF_OUT_MASK_LMK        ((1 << 1))
#endif

// Some hack defines for working without configure
#if defined(TAMER_1515_20)
#define LMK_TYPE				1010
#define LMX2531_TYPE 			1515
#define TAMER_OSC			20
#elif defined(TAMER_2080_20)
#define LMK_TYPE				1010
#define LMX2531_TYPE 			2080
#define TAMER_OSC			20
#elif defined(TAMER_2080_10)
#define LMK_TYPE				1010
#define LMX2531_TYPE 			2080
#define TAMER_OSC			10
#elif defined(TAMER_2080_VCTXCO_26_GPS)
#define LMK_TYPE				1010
#define LMX2531_TYPE 			2080
#define TAMER_OSC			26
#define PRESENT_GPS
#define PRESENT_DAC12
#elif defined(TAMER_1650_26_GPS)
#define LMK_TYPE				1010
#define LMX2531_TYPE 			1650
#define TAMER_OSC			26
#define PRESENT_GPS
#else
#error You should define ClockTamer hardware modification. There is no configure yet, sorry.
#endif

#define DEF_Fosc            1000000*TAMER_OSC

// Configuration helpers

#if LMX2531_TYPE == 1515

#define DEF_VCO_MIN         1450
#define DEF_VCO_MAX         1580
#define DEF_VCO_Kbit        2000

#elif LMX2531_TYPE == 1650

#define DEF_VCO_MIN         1590
#define DEF_VCO_MAX         1700
#define DEF_VCO_Kbit        2100

#elif LMX2531_TYPE == 2080

#define DEF_VCO_MIN         1904
#define DEF_VCO_MAX         2274
#define DEF_VCO_Kbit        4500

#elif LMX2531_TYPE == CUSTOM

#define DEF_VCO_MIN         LMX2531_VCO_MIN
#define DEF_VCO_MAX         LMX2531_VCO_MAX
#define DEF_VCO_Kbit        LMX2531_VCO_Kbit

#else
#error LMX2531_TYPE should be defined
#endif


// Defaults to 52Mhz if it isn't set
#ifndef DEF_Fout
#define DEF_Fout            52000000
#endif


#ifndef DEF_OUT_MASK_LMK
#define DEF_OUT_MASK_LMK		(1 << 6)
#endif




#endif //_TAMER_CONFIG_H_


