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

#include "Tamer.h"
#include "TamerBoard.h"
#include "lmx2531.h"
#include "lmk0x0xx.h"

#include <util/delay.h>
#include <avr/eeprom.h>

//#define PRESENT_DAC12
//#define DEBUG_REGS
//#define PRESENT_GPS


uint8_t SetLMX2531(uint8_t tuneOnly);

extern TamerCommand_t command;


extern RingBuff_t USARTtoUSB_Buffer;

uint8_t newLine[] PROGMEM = "\r\n";


void FillResultNoNewLinePM(uint8_t* res);

void FillResultPM(uint8_t* res)
{
    FillResultNoNewLinePM(res);
    FillResultNoNewLinePM(newLine);
}

void FillResultNoNewLinePM(uint8_t* res)
{
    uint8_t byte;
    while ((byte = pgm_read_byte(res++)))
    {
        Buffer_StoreElement(&USARTtoUSB_Buffer, byte);
    }
}

void FillHead(uint8_t* res, uint8_t idx)
{
    uint8_t byte;
    uint8_t i;
    res = res + 3*idx;
    for (i = 0; i < 3; i++)
    {
        byte = pgm_read_byte(res++);
        Buffer_StoreElement(&USARTtoUSB_Buffer, byte);
    }
}

extern uint8_t pCmd[];
extern uint8_t pTrg[];
extern uint8_t pDet[];

void FillCmd(void)
{
    if (command.cmd > 0)
        FillHead(pCmd, command.cmd - 1);
    Buffer_StoreElement(&USARTtoUSB_Buffer, ',');
    if (command.type > 0)
        FillHead(pTrg, command.type - 1);
    Buffer_StoreElement(&USARTtoUSB_Buffer, ',');
    if (command.details > 0)
        FillHead(pDet, command.details - 1);
    Buffer_StoreElement(&USARTtoUSB_Buffer, ',');
}

void FillUint32(uint32_t val)
{
    uint32_t stv = 1000000000;
    uint8_t f = 0;

    for (;stv > 0; stv/=10)
    {
        uint8_t v = val / stv;
       // if ((f) || (v > 0))
        {
            f = 1;
            Buffer_StoreElement(&USARTtoUSB_Buffer, '0' + v);
        }
        val -= v*stv;
    }
    if (f == 0)
        Buffer_StoreElement(&USARTtoUSB_Buffer, '0');
}

#ifdef FILL_UINT16
void FillUint16(uint16_t val)
{
    uint16_t stv = 10000;
    uint8_t f = 0;

    for (;stv > 0; stv/=10)
    {
        uint8_t v = val / stv;
        //if ((f) || (v > 0))
        {
            f = 1;
            Buffer_StoreElement(&USARTtoUSB_Buffer, '0' + v);
        }
        val -= v*stv;
    }
    if (f == 0)
        Buffer_StoreElement(&USARTtoUSB_Buffer, '0');
}
#else
#define FillUint16(x)   FillUint32(x)
#endif


uint8_t resOk[] PROGMEM = "OK";
uint8_t resErr[] PROGMEM = "ERROR";

uint8_t resVersion[] PROGMEM = "ClockTamer HW-1.0 API-1";
uint8_t resBadRange[] PROGMEM = "Bad tuning range";



//#define SERG_TAMER
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


//#define NOVARS

#ifdef NOVARS

#define  Fosc      DEF_Fosc
#define  Fout      DEF_Fout
#define  VCO_MIN   DEF_VCO_MIN
#define  VCO_MAX   DEF_VCO_MAX
#define  VCO_Kbit  DEF_VCO_Kbit

#else
uint32_t Fosc = DEF_Fosc;
uint32_t Fout = DEF_Fout;
uint16_t VCO_MIN =  DEF_VCO_MIN;
uint16_t VCO_MAX =  DEF_VCO_MAX;
uint16_t VCO_Kbit = DEF_VCO_Kbit;
#endif

uint8_t LMK_OutMask = 1 << 6;
uint8_t LMK_devider;
uint8_t AutoFreq;
#ifdef PRESENT_DAC12
uint16_t DacValue;
#endif

#ifdef PRESENT_GPS
uint8_t GpsSync_divider = 1;  // DIV
uint8_t PPS_skipped;       //KBT

uint32_t CounterHHValue;  //R00

uint16_t Count1PPS;       //R01

uint32_t LastOCPVal;      //R02

uint32_t FilteredVal;     //R03

uint32_t ddd;             //MAX

uint8_t AutoUpdateGps;    //AUT

uint32_t LastAutoUpd;     //MIN
#endif

#define GPSSYNC_MAX_FREQ        3000000


#ifdef DEBUG_REGS
uint32_t tmp_r0;
uint32_t tmp_r1;
uint32_t tmp_r2;
uint32_t tmp_r3;
uint32_t tmp_lmk;
#endif

uint32_t eeFosc         EEMEM = DEF_Fosc;
uint32_t eeFout         EEMEM = DEF_Fout;
uint16_t eeVCO_MIN      EEMEM = DEF_VCO_MIN;
uint16_t eeVCO_MAX      EEMEM = DEF_VCO_MAX;
uint16_t eeVCO_Kbit     EEMEM = DEF_VCO_Kbit;
uint8_t  eeLMK_OutMask  EEMEM = 1 << 6;
uint8_t  eeAutoFreq     EEMEM = 1;

uint16_t eeDacValue     EEMEM = 2048;

uint8_t  eeAutoGPSSync  EEMEM = 1;

void LoadEEPROM(void)
{
    Fosc = eeprom_read_dword(&eeFosc);
    Fout = eeprom_read_dword(&eeFout);

    VCO_MIN = eeprom_read_word(&eeVCO_MIN);
    VCO_MAX = eeprom_read_word(&eeVCO_MAX);
    VCO_Kbit = eeprom_read_word(&eeVCO_Kbit);

    LMK_OutMask = eeprom_read_byte(&eeLMK_OutMask);
    AutoFreq = eeprom_read_byte(&eeAutoFreq);

#ifdef PRESENT_DAC12
    DacValue = eeprom_read_word(&eeDacValue);
#endif

#ifdef PRESENT_GPS
    AutoUpdateGps = eeprom_read_byte(&eeAutoGPSSync);
#endif
}

void StoreEEPROM(void)
{
    eeprom_write_dword(&eeFosc, Fosc);
    eeprom_write_dword(&eeFout, Fout);

    eeprom_write_word(&eeVCO_MIN, VCO_MIN);
    eeprom_write_word(&eeVCO_MAX, VCO_MAX);
    eeprom_write_word(&eeVCO_Kbit, VCO_Kbit);

    eeprom_write_byte(&eeLMK_OutMask, LMK_OutMask);
    eeprom_write_byte(&eeAutoFreq, AutoFreq);

#ifdef PRESENT_DAC12
    eeprom_write_byte(&eeDacValue, DacValue);
#endif

#ifdef PRESENT_GPS
    eeprom_write_byte(&eeAutoGPSSync, AutoUpdateGps);
#endif
}

#ifdef PRESENT_GPS

void InitCounters(void)
{
    //External T1 Source
    TCCR1B = (1 << ICES1) | (1 << CS12) | (1 << CS11) | (1 << CS10);

    //Enable input capture interrupt
    TIMSK1 = (1 << ICIE1) | (1 << TOIE1);
}

#define FILTER_EXP_ALPHA        32


ISR(TIMER1_CAPT_vect, ISR_BLOCK)
{
    //TODO Impove this code!!!
    // Situation when counter overflows inside this inturrupt isn't covered

    if (Count1PPS % GpsSync_divider == 0)
    {
        uint32_t prev = LastOCPVal;

        LastOCPVal = ICR1;
        LastOCPVal |= (CounterHHValue << 16);
        if (TIFR1 & (1<< TOV1))
            LastOCPVal += 0x010000;

        uint32_t delta =  (LastOCPVal - prev);
        int32_t pint = (FilteredVal/FILTER_EXP_ALPHA - delta);
        if (pint < 0)
            pint = -pint;

        if ((Count1PPS != 0))
        {
            if ((FilteredVal == 0) || (PPS_skipped > 5))
            {
                PPS_skipped = 0;
                FilteredVal = FILTER_EXP_ALPHA*(delta);
            }
            else if ((uint32_t)pint < delta / 65536)
            {
                // TODO Impove this calculation
                FilteredVal = (FILTER_EXP_ALPHA-1)*(FilteredVal/FILTER_EXP_ALPHA) + (delta);

                PPS_skipped = 0;
            }
            else
                PPS_skipped++;

            ddd = pint;
        }
        else
        {
            PPS_skipped++;
        }
    }

    Count1PPS++;
}

ISR(TIMER1_OVF_vect, ISR_BLOCK)
{

    CounterHHValue++;
}

void UpdateOSCValue(void)
{
    int32_t delta = (int)(FilteredVal/FILTER_EXP_ALPHA) - (int)Fout ;

    uint32_t pdelta = (delta > 0) ? delta : -delta;
    if (pdelta < Fout / 4096)
    {
        uint32_t m = 0x7FFFFFFF / Fosc;
        if (pdelta > m)
        {
            delta = (delta > 0) ? m : -m;
        }

        Fosc = (uint32_t)((int32_t)Fosc + (delta*(int32_t)Fosc) / (int32_t)Fout);
    }
}

void TrimClock(void)
{
    //TODO Add atomic read for CounterHHValue

    // Check the mesuarements are stable
    if ((CounterHHValue - LastAutoUpd > 500) && (Count1PPS > 80) && (ddd < 60))
    {
        int32_t delta = (int)(FilteredVal/FILTER_EXP_ALPHA) - (int)Fout ;
        uint32_t pdelta = (delta > 0) ? delta : -delta;
        if (pdelta > 10)
        {
            UpdateOSCValue();

            LastAutoUpd = CounterHHValue;

            SetLMX2531(1);
        }

    }

}

#endif

void TamerControlAux(void)
{
#ifdef PRESENT_GPS
    if (AutoUpdateGps)
        TrimClock();
#endif
}

void InitLMX2531(void)
{
    // Enable LMX2531
    LmxCeSet();

    _delay_ms(50);

    LMX2531_WRITE(LMX2531_R5_INIT1);
    LMX2531_WRITE(LMX2531_R5_INIT2);
    LMX2531_WRITE(LMX2531_R5_INIT3);

    _delay_ms(10);

    LMX2531_WRITE(LMX2531_R12_INIT);
    LMX2531_WRITE(LMX2531_R9_INIT);
}

void InitLMK(void)
{
    // Enable LMK01000
    LmkGoeSet();
    _delay_ms(50);

    LMK0X0XX_WRITE(0x80000100);
}

#define MAKE_UINT32T(a,b,c,d)      \
    ( ((uint32_t)(a)<<24)  |       \
      ((uint32_t)(b)<<16)  |       \
      ((uint16_t)(c)<<8)   |       \
      (d) )

#define MAKE_UINT16T(c,d)      \
      (((uint16_t)(c)<<8)   |  \
      (d) )


#define den_bit                 21
#define den         ((uint32_t)1 << den_bit)

#define DIVIDER_MAX_FREQ        1500
//#define DIVIDER_MAX_FREQ        1590


uint8_t SetLMX2531(uint8_t tuneOnly)
{
    uint32_t num;

    uint8_t r = 1;
    if (Fosc > 14000)
       r = 2;

    uint16_t vco;
    //uint16_t foutmhz = Fout / 1000000;
    uint16_t foutmhz = Fout / 100000;

    if (foutmhz == 0)
        return 0;

    uint16_t i;

    if (VCO_MAX > DIVIDER_MAX_FREQ)
    {
        i = (((uint16_t)10*(VCO_MIN + VCO_MAX) / 8) / (foutmhz)) * 4;
        vco = (i + 4) * (foutmhz);
        if (vco > 10*VCO_MAX)
            vco = i * (foutmhz);
        else
            i+=4;
    }
    else
    {
      i = (((uint16_t)10*(VCO_MIN + VCO_MAX) / 4) / (foutmhz)) * 2;
      vco = (i + 2) * (foutmhz);
      if (vco > 10*VCO_MAX)
          vco = i * (foutmhz);
      else
          i+=2;
    }
    if (vco < 10*VCO_MIN)
    {
        return 0;
    }


    uint16_t n = (uint32_t)(i*Fout) / (Fosc / r);
    uint32_t rem_n = ((uint32_t)(i*Fout))  % (Fosc / r);


    //tmp_r0 = n;
    //tmp_r1 = rem_n;

#ifdef DIV64
    num = ((uint64_t)den * rem_n) / ((Fosc));

#else
    uint32_t dev1 = (rem_n ) / ((Fosc / r));
    uint32_t rem1 = (rem_n ) % ((Fosc / r));
    num = dev1;

    uint8_t t;
    for (t = 0; t < den_bit; t++)
    {
        rem1 <<= 1;
        num <<= 1;

        dev1 = (rem1) / ((Fosc / r));
        rem1 = (rem1) % ((Fosc / r));

        if (dev1 & 1)
            num |= 1;
    }
#endif


    if (VCO_MAX > DIVIDER_MAX_FREQ)
        i /= 2;

    if (i > 510)
        return 0;

    if (!tuneOnly)
    {
        LMX2531_WRITE( MAKE_R8(LOCKMODE_LINEAR, Fosc/1000) );
        LMX2531_WRITE( MAKE_R7(VCO_Kbit, Fosc/1000) );
        LMX2531_WRITE( MAKE_R6(XTLSEL_MANUAL, VCO_ACI_SEL_M1, 1, R_40, R_10, R_40, R_10, C3_C4_100_100) );
        //LMX2531_WRITE( MAKE_R6(CALC_XTSEL(Fosc/1000000), VCO_ACI_SEL_M1, 1, R_40, R_10, R_40, R_10, C3_C4_100_100) );
        LMX2531_WRITE( MAKE_R4(ICP_1X, TOC_DISABLED) );
#ifdef DEBUG_REGS
        LMX2531_WRITE(tmp_r3 = MAKE_R3((VCO_MAX > DIVIDER_MAX_FREQ), FDM_FRACTIONAL, DITHER_STRONG, FRAC_ORDER_4, FOLD_DISABLED, den >> 12) );
        LMX2531_WRITE(tmp_r2 = MAKE_R2(den & 0xFFF, r) );
    }
    LMX2531_WRITE(tmp_r1 = MAKE_R1(ICP_1X, n >> 8, num >> 12) );
    LMX2531_WRITE(tmp_r0 = MAKE_R0(n & 0xFF, num & 0xFFF) );
#else
        LMX2531_WRITE( MAKE_R3((VCO_MAX > DIVIDER_MAX_FREQ), FDM_FRACTIONAL, DITHER_STRONG, FRAC_ORDER_4, FOLD_DISABLED, den >> 12) );
        LMX2531_WRITE( MAKE_R2(den & 0xFFF, r) );
    }
    LMX2531_WRITE( MAKE_R1(ICP_1X, n >> 8, num >> 12) );
    LMX2531_WRITE( MAKE_R0(n & 0xFF, num & 0xFFF) );
#endif

    LMK_devider = i/2;

#ifdef PRESENT_GPS
    GpsSync_divider = Fout / GPSSYNC_MAX_FREQ;
    if (GpsSync_divider < 0)
        GpsSync_divider = 1;

    uint16_t tmp = GpsSync_divider * LMK_devider;
    if (tmp > 255)
        return 0;
#endif

    return 1;
}


void SetLMK(void)
{
/*
   uint8_t j = 1;
   for (uint8_t i = 0; i < 8; i++, j<<=1)
   {
        if ((j & LMK_OutMask) == j)
            LMK0X0XX_WRITE(MAKE_LMK(1, 1, LMK_devider/2, 0, i));
        else
            LMK0X0XX_WRITE(0x00000100 | i);
   }
*/


   LMK0X0XX_WRITE(0x00000101);
   LMK0X0XX_WRITE(0x00000102);
   LMK0X0XX_WRITE(0x00000103);
   LMK0X0XX_WRITE(0x00000104);
   LMK0X0XX_WRITE(0x00000105);
   //LMK0X0XX_WRITE(0x00030AF6);
   //LMK0X0XX_WRITE(0x00030806);
#ifdef DEBUG_REGS
   LMK0X0XX_WRITE(tmp_lmk = MAKE_LMK(1, 1, LMK_devider, 0, 6));
#else
   LMK0X0XX_WRITE(MAKE_LMK(1, 1, LMK_devider, 0, 6));
#endif

#ifdef PRESENT_GPS
   LMK0X0XX_WRITE(MAKE_LMK(1, 1, GpsSync_divider * LMK_devider, 0, 4));
#endif

   LMK0X0XX_WRITE(0x00000107);
   LMK0X0XX_WRITE(0x00022A09);
   LMK0X0XX_WRITE(0x6800000E);



   _delay_ms(10);

    LmkSyncClear();

   _delay_ms(10);

    LmkSyncSet();
}



void AutoStartControl(void)
{
#ifdef PRESENT_DAC12
    DacSyncInit();
    DacSyncSet();
#endif

#ifdef PRESENT_GPS
    InitCounters();
#endif

    AutoFreq = eeprom_read_byte(&eeAutoFreq);
    if (AutoFreq)
    {
        LoadEEPROM();

        InitLMX2531();

        InitLMK();

        _delay_ms(50);

        if (SetLMX2531(0))
        {
            SetLMK();
        }
    }
}

#ifdef PRESENT_DAC12
uint8_t SetDac(uint16_t value)
{
    if (value < 0x1000)
    {
        DAC12_WRITE(value);
        return 1;
    }

    return 0;
}
#endif

void ProcessCommand(void)
{
    switch(command.cmd)
    {
        case cmdIDLE:
            return;

        case cmdREG:
        {

            switch (command.type)
            {
                case trgLMK:
                    write_reg_LMK0X0XX(command.data[3], command.data[2], command.data[1], command.data[0]);
                    FillResultPM(resOk);
                    return;

                case trgLMX:
                    write_reg_LMX2531(command.data[2], command.data[1], command.data[0]);
                    FillResultPM(resOk);
                    return;
#ifdef PRESENT_DAC12
                case trgDAC:
                    switch (command.details)
                    {
                        case detD12:
                            write_reg_DAC12(command.data[1], command.data[0]);
                            return;
                        default:
                            FillResultPM(resErr);
                            return;
                    }
#endif
                default:
                    FillResultPM(resErr);
            }
            break;
        }

        case cmdPIN:
        {
            switch (command.type)
            {
                case trgLMK:
                {
                    switch (command.details)
                    {
                        case detGOE:
                            if (command.data[0])
                                LmkGoeSet();
                            else
                                LmkGoeClear();
                            break;
                        case detSYN:
                            if (command.data[0])
                                LmkGoeSet();
                            else
                                LmkGoeClear();
                            break;
                        default:
                            FillResultPM(resErr);
                            return;
                    }
                    FillResultPM(resOk);
                    return;
                }
                case trgLMX:
                {
                    switch (command.details)
                    {
                        case detEN:
                            if (command.data[0])
                                LmxCeSet();
                            else
                                LmxCeClear();
                            break;
                        default:
                            FillResultPM(resErr);
                            return;
                    }
                    FillResultPM(resOk);
                    return;
                }

                case trgLED:
                {
                    if (command.data[0])
                        LedSet();
                    else
                        LedClear();

                    FillResultPM(resOk);
                    break;
                }
                default:
                    FillResultPM(resErr);
            }
            break;

        }

        case cmdSET:
        {
            switch (command.type)
            {
                case trgNONE:
                    switch (command.details)
                    {
                        case trgNONE:
                            break;
                        case detOUT:
                            Fout = command.u32data;
                            break;
                        case detOSC:
                            Fosc = command.u32data;
                            break;
                        case detAUTO:
                            AutoFreq = command.data[0];
                            break;
                        default:
                            FillResultPM(resErr);
                            return;
                    }

                    uint8_t r = SetLMX2531(0);
                    if (r)
                    {
                        SetLMK();
                        FillResultPM(resOk);
                    }
                    else
                    {
                        FillResultPM(resBadRange);
                    }
                    break;

                case trgVCO:
                    switch (command.details)
                    {
                        case detMIN:
                            VCO_MIN = command.u16data[0];
                            break;
                        case detMAX:
                            VCO_MAX = command.u16data[0];
                            break;
                        case detKBIT:
                            VCO_Kbit = command.u16data[0];
                            break;

                        default:
                            FillResultPM(resErr);
                            return;
                    }

                    FillResultPM(resOk);
                    break;

                case trgLMK:
                    switch (command.details)
                    {
                        case detPORTS:
                            LMK_OutMask = command.data[0];
                            break;

                        default:
                            FillResultPM(resErr);
                            return;
                    }

                    SetLMK();

                    FillResultPM(resOk);
                    break;
#ifdef PRESENT_DAC12
                case trgDAC:
                    switch (command.details)
                    {
                        case detNONE:
                            break;

                        case detD12:
                            DacValue = command.u16data[0];
                            break;

                        default:
                            FillResultPM(resErr);
                            return;
                    }

                    if (SetDac(DacValue))
                        FillResultPM(resOk);
                    else
                        FillResultPM(resBadRange);
                    return;
#endif

#ifdef PRESENT_GPS
                case trgGPS:
                {
                    switch (command.details)
                    {
                        case detSYN:    UpdateOSCValue(); break;
                        case detAUTO:   AutoUpdateGps = command.data[0]; break;
                        default:
                            FillResultPM(resErr);
                            return;
                    }

                    FillResultPM(resOk);
                    return;
                }
#endif
                default:
                    FillResultPM(resErr);
                    break;
            }
            break;
        }

        case cmdINFO:
        {
            switch (command.type)
            {
#ifdef PRESENT_GPS
                case trgGPS:
                {
                    switch (command.details)
                    {
                        case detDIVIDERS: FillCmd();  FillUint16(GpsSync_divider);  FillResultNoNewLinePM(newLine); break;
                        case detKBIT:     FillCmd();  FillUint16(PPS_skipped);      FillResultNoNewLinePM(newLine); break;
                        case detR00:      FillCmd();  FillUint32(CounterHHValue);   FillResultNoNewLinePM(newLine); break;
                        case detR01:      FillCmd();  FillUint16(Count1PPS);        FillResultNoNewLinePM(newLine); break;
                        case detR02:      FillCmd();  FillUint32(LastOCPVal);       FillResultNoNewLinePM(newLine); break;
                        case detR03:      FillCmd();  FillUint32(FilteredVal);      FillResultNoNewLinePM(newLine); break;
                        case detMAX:      FillCmd();  FillUint32(ddd);              FillResultNoNewLinePM(newLine); break;
                        case detAUTO:     FillCmd();  FillUint16(AutoUpdateGps);    FillResultNoNewLinePM(newLine); break;
                        case detMIN:      FillCmd();  FillUint32(LastAutoUpd);      FillResultNoNewLinePM(newLine); break;
                        default: break;
                    }

                    break;
                }
#endif
                case trgNONE:
                {
                    switch (command.details)
                    {
                        case detOUT:   FillCmd();  FillUint32(Fout);      FillResultNoNewLinePM(newLine); break;
                        case detOSC:   FillCmd();  FillUint32(Fosc);      FillResultNoNewLinePM(newLine); break;
                        case detAUTO:  FillCmd();  FillUint16(AutoFreq);  FillResultNoNewLinePM(newLine); break;
                        default: break;
                    }
                    break;
                }

                case trgVCO:
                {
                    switch (command.details)
                    {
                        case detMIN:   FillCmd();  FillUint16(VCO_MIN);   FillResultNoNewLinePM(newLine); break;
                        case detMAX:   FillCmd();  FillUint16(VCO_MAX);   FillResultNoNewLinePM(newLine); break;
                        case detKBIT:  FillCmd();  FillUint16(VCO_Kbit);  FillResultNoNewLinePM(newLine); break;
#ifdef DEBUG_REGS
                        case detR00:   FillCmd();  FillUint32(tmp_r0);      FillResultNoNewLinePM(newLine); break;
                        case detR01:   FillCmd();  FillUint32(tmp_r1);      FillResultNoNewLinePM(newLine); break;
                        case detR02:   FillCmd();  FillUint32(tmp_r2);      FillResultNoNewLinePM(newLine); break;
                        case detR03:   FillCmd();  FillUint32(tmp_r3);      FillResultNoNewLinePM(newLine); break;
                        case detEN:    FillCmd();  FillUint32(tmp_lmk);     FillResultNoNewLinePM(newLine); break;
#endif
                        default: break;
                    }
                    break;
                }

                case trgLMK:
                {
                    switch (command.details)
                    {
                        case detPORTS:    FillCmd();  FillUint16(LMK_OutMask);  FillResultNoNewLinePM(newLine); break;
                        case detDIVIDERS: FillCmd();  FillUint16(LMK_devider);  FillResultNoNewLinePM(newLine); break;
                        default: break;
                    }
                    break;
                }

#ifdef PRESENT_DAC12
                case trgDAC:  FillCmd();  FillUint16(DacValue);  FillResultNoNewLinePM(newLine); break;
#endif

                default:
                  break;
            }

            FillResultPM(resOk);
            break;
        }

        case cmdRST:
        {
            InitLMX2531();
            InitLMK();
            FillResultPM(resOk);
            break;
        }

        case cmdVERSION:
            FillResultPM(resVersion);
            break;

        case cmdLDE:
            LoadEEPROM();
            FillResultPM(resOk);
            break;

        case cmdSTE:
            StoreEEPROM();
            FillResultPM(resOk);
            break;

        default:
            FillResultPM(resErr);

    }



}