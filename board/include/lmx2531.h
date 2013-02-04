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

#ifndef LMX2531
#define LMX2531

// For embedded devices
#ifndef int24_t
#define int24_t  int32_t
#endif

#define MAKE_REG(r,v) (((r) & 0xf) && (((v) & 0xfffff) << 4))

#define LMX2531_R5_INIT1        0x840005
#define LMX2531_R5_INIT2        0x800005
#define LMX2531_R5_INIT3        0x8007F5
#define LMX2531_R9_INIT         0x000BA9
#define LMX2531_R12_INIT        0x01048C


// < 20 MHz
#define XTLDIV_DIV2     0x1
// 20 - 40 Mhz
#define XTLDIV_DIV4     0x2
// > 40 Mhz
#define XTLDIV_DIV8     0x3


#define LOCKMODE_LINEAR          1

/// fosc in khz
#define CALC_XTLMAN(bit, fosc)  (0xfff & (((fosc)<<4)/(bit)))
#define CALC_XTLDIV(fosc)       ( (fosc) < 40000 ? (((fosc) / 20000)+1) : 3)

#define MAKE_R7(bit, fosc)      ((( ((int24_t)CALC_XTLMAN(bit, fosc) << 2) | CALC_XTLDIV(fosc)) << 8) | 7)
#define MAKE_R8(mode, fosc)           ( (((int24_t)0x4 | mode) << 15) | ((fosc) < 40000 ? 0 : 0x10) | 8)


#define C3_C4_50_50              0
#define C3_C4_50_100             1
#define C3_C4_50_150             2
#define C3_C4_100_50             3
#define C3_C4_150_50             4
#define C3_C4_100_100            5

#define R_10                     0
#define R_20                     1
#define R_30                     2
#define R_40                     3


//for all except 2265, 2570
#define VCO_ACI_SEL_M1           8
#define VCO_ACI_SEL_M2           6

#define XTLSEL_MANUAL            4


#define CALC_XTSEL(foschz)         ((foschz) < 70 ? ((foschz) / 25): 3)

#define MAKE_R6(xtsel, vaci, en, r4, r4fl, r3, r3fl, cec4)   \
    (((((((((((((((int24_t) (xtsel) ) << 4) | (vaci)) << 1) | (en) ) << 2) | (r4)) << 2 | (r4fl)) << 2 | (r3)) << 2 | (r3fl)) << 3) | (cec4)) << 4) | 6)


#define FRAC_ORDER_4             0
#define FRAC_ORDER_0             1
#define FRAC_ORDER_2             2
#define FRAC_ORDER_3             3

#define DITHER_WEAK              0
#define DITHER_STRONG            2
#define DITHER_DISABLED          3

#define DIV2_ENABLED             1

#define FOLD_DISABLED            0
#define FOLD_DIGITAL_HIGH        1
#define FOLD_DIGITAL_LOW         2
#define FOLD_DIGITAL_LOCK        3


#define FDM_INTEGER              0
#define FDM_FRACTIONAL           1

#define ICP_1X                   0
#define ICP_2X                   1
#define ICP_3X                   2
#define ICP_4X                   3

#define ICP_5X                   4
#define ICP_6X                   5
#define ICP_7X                   6
#define ICP_8X                   7

#define TOC_DISABLED             0

#define MAKE_R4(icpfl, toc) \
        ((((((int24_t)(icpfl)) << 14) | (toc)) << 4) | 4)

#define MAKE_R3(div, fdm, dither, order, fold, denh) \
        (((((((((((((int24_t) (div)) << 1 | (fdm)) << 2) | (dither)) << 2 ) | (order) ) << 4) | (fold)) << 10) | (denh)) << 4) | 3)

#define MAKE_R2(denl, r) \
        ((((((int24_t)0x1000 | (denl)) << 6) | (r)) << 4) | 2)

#define MAKE_R1( icp, nh, numh) \
    ((((((( (int24_t)0x10 | (icp)) << 3) | (nh)) << 10) | (numh)) << 4) | 1)

#define MAKE_R0(nl, numl) \
    ((((int24_t) (nl) << 12) | (numl)) << 4)



#endif
