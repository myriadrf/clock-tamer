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

//gcc lmx2531regtest.c -o lmx2531regtest -I../include

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "lmx2531.h"
#include "lmk0x0xx.h"

#define CLOCK_TAMER

#ifdef CLOCK_TAMER

//#define DEF_Fosc            26000035
#define DEF_Fosc            20000000
#define DEF_Fout            52000000

#define DEF_VCO_MIN         1450
#define DEF_VCO_MAX         1580

// LMX1515E
#define DEF_VCO_Kbit        2000

#elif defined(SERG_TAMER)

//#define DEF_Fosc            26000035
#define DEF_Fosc            26000000
#define DEF_Fout            52000000

#define DEF_VCO_MIN         1590
#define DEF_VCO_MAX         1700
// LMX1515E
//#define Kbit              2000
// LMX1650E
#define DEF_VCO_Kbit        2100
// LMX2080E
//#define Kbit              4500
#else
// ALEX_TAMER
#define DEF_Fosc            9999983
#define DEF_Fout            52000000

#define DEF_VCO_MIN         1904
#define DEF_VCO_MAX         2274

#define DEF_VCO_Kbit        4500
#endif

int main(int argc, char* argv[])
{
    // 
    uint32_t Fosc = DEF_Fosc;
    uint32_t Fout = DEF_Fout;
    uint16_t VCO_MIN = DEF_VCO_MIN;
    uint16_t VCO_MAX = DEF_VCO_MAX;
    uint16_t Kbit = DEF_VCO_Kbit;

    printf("usage: Fosc(hz)  Fout(hz)  VCO_MIN(mhz)  VCO_MAX(mhz)\n");

    if (argc > 1)
    {
        Fosc = atoi(argv[1]);
        if (argc > 2)
        {
            Fout = atoi(argv[2]);
            if (argc > 3)
            {
                VCO_MIN = atoi(argv[3]);
                if (argc > 4)
                {
                    VCO_MAX  = atoi(argv[4]);
                }
            }
        }
    }

    printf("Fosc = %d Hz, Fout = %d Hz, VCO_MIN = %d MHz, VCO_MAX = %d MHz\n\n",
          Fosc, Fout, VCO_MIN, VCO_MAX);

    int osc = CALC_XTSEL(Fosc);
    printf("CALC_XTSEL(Fosc) = %03x (%d)\n", osc, Fosc);
    int xtalman = CALC_XTLMAN(Kbit, Fosc/1000);
    printf("CALC_XTLMAN(Kbit, Fosc) = %d (%d)\n", xtalman, Kbit);

    //uint32_t den = 2500000;
    uint32_t num;

    uint8_t r = 1;
    if (Fosc > 14000)
       r = 2;

    uint16_t vco;
    uint16_t foutmhz = Fout / 100000;

    uint16_t i;


    if (VCO_MAX > 1600)
    {
        i = ((10*(VCO_MIN + VCO_MAX) / 8) / (foutmhz)) * 4;
        vco = (i + 4) * (foutmhz);
        if (vco > 10*VCO_MAX)
            vco = i * (foutmhz);
        else
            i+=4;
    }
    else
    {
      i = ((10*(VCO_MIN + VCO_MAX) / 4) / (foutmhz)) * 2;
      vco = (i + 2) * (foutmhz);
      if (vco > 10*VCO_MAX)
          vco = i * (foutmhz);
      else
          i+=2;
    }
    if (vco < 10*VCO_MIN)
        return 0;


#define den  (1<<21)

    uint16_t n = (uint32_t)(i*Fout) / (Fosc / r);
    uint32_t rem_n = (uint32_t)(i*Fout)  % ((Fosc/r));

    num = ((uint64_t)den * rem_n) / ((Fosc/r));
    printf ("i = %d \nFosc =%d,%x   n=%d  rem_n = %d,%x       num = %d, %x   (%f)\n", i, Fosc,Fosc, n, rem_n, rem_n, num, num, (double)num / den);


    uint32_t dev1 = (rem_n ) / ((Fosc/r));
    uint32_t rem1 = (rem_n ) % ((Fosc/r));

    uint32_t val = dev1;

int t;
for (t = 0; t < 21; t++)
{
    rem1 <<= 1;
    val <<= 1;

    dev1 = (rem1) / ((Fosc/r));
    rem1 = (rem1) % ((Fosc/r));

    if (dev1 & 1)
        val |= 1;

    printf ("dev1 = %d,%x  rem1 = %d,%x   val = %d,%x\n", dev1, dev1, rem1, rem1, val, val);

}   


    uint32_t reg8 = MAKE_R8(LOCKMODE_LINEAR, Fosc/1000);
    uint32_t reg7 = MAKE_R7(Kbit, Fosc/1000);
    uint32_t reg6 = MAKE_R6(CALC_XTSEL(Fosc/1000000), VCO_ACI_SEL_M1, 1, R_40, R_10, R_40, R_10, C3_C4_100_100);
    uint32_t reg4 = MAKE_R4(ICP_1X, TOC_DISABLED);
    uint32_t reg3 = MAKE_R3((VCO_MAX > 1600), FDM_FRACTIONAL, DITHER_STRONG, FRAC_ORDER_4, FOLD_DISABLED, den >> 12);
    uint32_t reg2 = MAKE_R2(den & 0xFFF, r);
    uint32_t reg1 = MAKE_R1(ICP_1X, n >> 8, num >> 12);
    uint32_t reg0 = MAKE_R0(n & 0xFF, num & 0xFFF);


    if (VCO_MAX > 1600)
      i /= 2;
    printf ("VCO = %d, FOUT = %d,  i = %d\n", vco, (Fout * i) / 1000 , i);
    printf ("R = %d, N = %d, { num = %d; den = %d } (%f)\n", r, n, num, den,(double)num / den);


    printf("R8 = %06x\n", reg8);
    printf("R7 = %06x\n", reg7);
    printf("R6 = %06x\n", reg6);

    printf("R4 = %06x\n", reg4);
    printf("R3 = %06x (%d)\n", reg3, reg3);
    printf("R2 = %06x (%d)\n", reg2, reg2);
    printf("R1 = %06x (%d)\n", reg1, reg1);
    printf("R0 = %06x (%d)\n", reg0, reg0);



    printf("LMK R6 = %08x  (%d)\n", MAKE_LMK(1, 1, i/2, 0, 6), MAKE_LMK(1, 1, i/2, 0, 6));

    return 0;
}