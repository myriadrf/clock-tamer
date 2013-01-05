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

#include <stdint.h>

#include "Tamer.h"
#include "TamerBoard.h"
#include "lmx2531.h"
#include "lmk0x0xx.h"

#include "TamerControl.h"
#include "TamerConfig.h"

#include <util/delay.h>
#include <avr/eeprom.h>

#define BIND static
#include "uint64_ops.h"

//////////////////////////////////////
#define SELF_TESTING
//////////////////////////////////////

#if   TAMER_VER >= 12
#define BLINK_1PPS
#define GPS_PORT 2
#elif TAMER_VER == 11
#define GPS_PORT 3
#else
#define GPS_PORT 4
#endif

#define VCO_FIXED
//#define NO_VERSION
//#define NO_HWINFO
//#define NO_CMDINFO
//#define NO_CMDPIN
//#define NO_CMDREG
//#define NO_CMDEELOAD
#define UNROLL_MACROSS_LMX2531

#define DEBUG_REGS


#define DIVIDER_MAX_FREQ        1500
//#define DIVIDER_MAX_FREQ        1590

#define den_bit                 21
#define den         ((uint32_t)1 << den_bit)


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

static void FillHead(uint8_t* res, uint8_t idx)
{
    uint8_t byte;
    uint8_t i;

    if (idx > 0xf0)
    {
        Buffer_StoreElement(&USARTtoUSB_Buffer, '?');
    }
    else
    {
        res = res + 3*idx;
        for (i = 0; i < 3; i++)
        {
            byte = pgm_read_byte(res++);
            Buffer_StoreElement(&USARTtoUSB_Buffer, byte);
        }
    }
}

extern uint8_t pCmd[];
extern uint8_t pTrg[];
extern uint8_t pDet[];

static void FillCmd(void)
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

static void FillUint32(uint32_t val)
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
static void FillUint16(uint16_t val)
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

#ifndef NO_VERSION
#if TAMER_VER == 123
uint8_t resVersion[] PROGMEM = "ClockTamer SW=1.23 API=1";
#elif TAMER_VER == 122
uint8_t resVersion[] PROGMEM = "ClockTamer SW=1.22 API=1";
#elif TAMER_VER == 121
uint8_t resVersion[] PROGMEM = "ClockTamer SW=1.21 API=1";
#elif TAMER_VER == 12
uint8_t resVersion[] PROGMEM = "ClockTamer SW=1.2 API=1";
#elif TAMER_VER == 11
uint8_t resVersion[] PROGMEM = "ClockTamer SW=1.1 API=1";
#elif TAMER_VER == 10
uint8_t resVersion[] PROGMEM = "ClockTamer SW=1.0 API=1";
#else
uint8_t resVersion[] PROGMEM = "ClockTamer SW=[unknown] API=1";
#endif
#endif

uint8_t resBadRange[] PROGMEM = "Bad tuning range";



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

#ifdef VCO_FIXED
#define  VCO_MIN   DEF_VCO_MIN
#define  VCO_MAX   DEF_VCO_MAX
#define  VCO_Kbit  DEF_VCO_Kbit

#else
uint16_t VCO_MIN =  DEF_VCO_MIN;
uint16_t VCO_MAX =  DEF_VCO_MAX;
uint16_t VCO_Kbit = DEF_VCO_Kbit;
#endif
#endif



uint8_t LMK_OutMask = DEF_OUT_MASK_LMK;
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

#if TAMER_VER >= 12
uint8_t EnableOscillator = 1;
#endif

#ifdef SELF_TESTING
uint8_t VCO_locked = 0;
#ifdef PRESENT_GPS
uint8_t GPS_locked = 0;
#endif

uint8_t SelfStage = 0;
uint8_t SelfStageMax = 0;

uint32_t SelfMin = 0;
uint32_t SelfMax = 0;
uint32_t SelfPrev = 0;


#define FOLD_VALUE  FOLD_DIGITAL_LOCK

static inline uint8_t IsVcoLocked()
{
    return (PINC & (1<<PC5));
}
#else //SELF_TESTING
#define FOLD_VALUE FOLD_DISABLED
#endif

//#define GPSSYNC_MAX_FREQ        3000000
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

#ifndef VCO_FIXED
uint16_t eeVCO_MIN      EEMEM = DEF_VCO_MIN;
uint16_t eeVCO_MAX      EEMEM = DEF_VCO_MAX;
uint16_t eeVCO_Kbit     EEMEM = DEF_VCO_Kbit;
#else
uint16_t unused_eeVCO_MIN      EEMEM = DEF_VCO_MIN;
uint16_t unused_eeVCO_MAX      EEMEM = DEF_VCO_MAX;
uint16_t unused_eeVCO_Kbit     EEMEM = DEF_VCO_Kbit;
#endif

uint8_t  eeLMK_OutMask  EEMEM = DEF_OUT_MASK_LMK;
uint8_t  eeAutoFreq     EEMEM = 1;

uint16_t eeDacValue     EEMEM = 2048;

uint8_t  eeAutoGPSSync  EEMEM = 1;

uint8_t  eeEnableOscillator  EEMEM = 1;

static void LoadEEPROM(void)
{
    Fosc = eeprom_read_dword(&eeFosc);
    Fout = eeprom_read_dword(&eeFout);

#ifndef VCO_FIXED
    VCO_MIN = eeprom_read_word(&eeVCO_MIN);
    VCO_MAX = eeprom_read_word(&eeVCO_MAX);
    VCO_Kbit = eeprom_read_word(&eeVCO_Kbit);
#endif

    LMK_OutMask = eeprom_read_byte(&eeLMK_OutMask);
    AutoFreq = eeprom_read_byte(&eeAutoFreq);

#ifdef PRESENT_DAC12
    DacValue = eeprom_read_word(&eeDacValue);
#endif

#ifdef PRESENT_GPS
    AutoUpdateGps = eeprom_read_byte(&eeAutoGPSSync);
#endif

#if TAMER_VER >= 12
    EnableOscillator = eeprom_read_byte(&eeEnableOscillator);
#endif
}

static void StoreEEPROM(void)
{
    eeprom_write_dword(&eeFosc, Fosc);
    eeprom_write_dword(&eeFout, Fout);

#ifndef VCO_FIXED
    eeprom_write_word(&eeVCO_MIN, VCO_MIN);
    eeprom_write_word(&eeVCO_MAX, VCO_MAX);
    eeprom_write_word(&eeVCO_Kbit, VCO_Kbit);
#endif
    eeprom_write_byte(&eeLMK_OutMask, LMK_OutMask);
    eeprom_write_byte(&eeAutoFreq, AutoFreq);

#ifdef PRESENT_DAC12
    eeprom_write_byte(&eeDacValue, DacValue);
#endif

#ifdef PRESENT_GPS
    eeprom_write_byte(&eeAutoGPSSync, AutoUpdateGps);
#endif

#if TAMER_VER >= 12
    eeprom_write_byte(&eeEnableOscillator, EnableOscillator);
#endif

}

static void LoadHwInfo(void)
{
    int i;
    for (i=0; i<HWI_LEN; i++)
    {
        char c = eeprom_read_byte(&eeHWInfo[i]);
        if (c == 0)
            break;
        Buffer_StoreElement(&USARTtoUSB_Buffer, c);
    }
	FillResultNoNewLinePM(newLine);
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

#define RESET_OCP_COUNTER       0x80

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
            if (PPS_skipped == RESET_OCP_COUNTER)
            {
                PPS_skipped++;
            }
            else if ((FilteredVal == 0) || (PPS_skipped > 1))
            {
                PPS_skipped = 0;
                FilteredVal = FILTER_EXP_ALPHA*(delta);
            }
            else if ((uint32_t)pint < delta / 524288)
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

#ifdef BLINK_1PPS
    INFOLED_PORT ^= (1 << INFOLED);
#endif
}

ISR(TIMER1_OVF_vect, ISR_BLOCK)
{
    CounterHHValue++;
}

#ifdef SELF_TESTING
//uint8_t cnt;

// Will l
#define SUMM_CNT 128

//Will alarm each 256K Cycles
ISR(TIMER0_OVF_vect, ISR_BLOCK)
{
    if (++SelfStage == SelfStageMax) {
        uint32_t prev = SelfPrev;
        SelfPrev = TCNT1;
        SelfPrev |= (CounterHHValue << 16);        
        if (TIFR1 & (1<< TOV1))
            SelfPrev += 0x010000;

        SelfStage = 0;

        if (prev == 0) {
            //Initial
        } else {
            uint32_t delta = SelfPrev - prev;
            if (SelfMin > delta)
                SelfMin = delta;
            else if (SelfMax < delta)
                SelfMax = delta;
        }
    }
}

static void SelfTestFlush()
{
    cli();
    SelfStage = SelfStageMax - 1;
    SelfPrev = 0;
    SelfMax = 0;
    SelfMin = 0xfffffffful;
    sei();
}

static void SelfTestStart(uint8_t max)
{
    SelfStageMax = max;
    //SET divider to max
    TCNT0 = 0;
    TCCR0B = (1 << CS02) | (1 << CS00); //Freq/1024
    SelfTestFlush();

    TIMSK0 = 1 << TOIE0;
}
static void SelfTestStop()
{
    TIMSK0 = 0;
}

static uint32_t SelfToOutputFreq(uint32_t v)
{
    //return 0;
    //return ((uint64_t)(GpsSync_divider * v) * 8 * 1000000) / (1024 * 256 * SelfStageMax);

    return uint64_div32(uint64_mul32(v, (uint32_t)GpsSync_divider * 8 * 1000000),
                        (uint32_t)SelfStageMax * 1024 * 256);

}


/**
 * @brief SelfTestLockPin
 * @return 1 - Ok, 0 - Pin FOLD doesn't work
 */
static uint8_t SelfTestLockPin()
{
    uint8_t res;

    LMX2531_WRITE( MAKE_R3((VCO_MAX > DIVIDER_MAX_FREQ), FDM_FRACTIONAL, DITHER_STRONG, FRAC_ORDER_4, FOLD_DIGITAL_LOW,  den >> 12) );
    _delay_ms(10);

    res = !IsVcoLocked(); //Must be low

    LMX2531_WRITE( MAKE_R3((VCO_MAX > DIVIDER_MAX_FREQ), FDM_FRACTIONAL, DITHER_STRONG, FRAC_ORDER_4, FOLD_DIGITAL_HIGH,  den >> 12) );
    _delay_ms(10);

    res = res && IsVcoLocked(); //Must be high

    LMX2531_WRITE( MAKE_R3((VCO_MAX > DIVIDER_MAX_FREQ), FDM_FRACTIONAL, DITHER_STRONG, FRAC_ORDER_4, FOLD_DIGITAL_LOCK,  den >> 12) );
    return res;
}

uint8_t stPragma[] PROGMEM = "SEL,F_T,EST,";
uint8_t stLck[]  PROGMEM = "lock pin,";
uint8_t stFreq[]  PROGMEM = "set freq,";
uint8_t stCntd[]  PROGMEM = "counted,";
uint8_t stComma[] PROGMEM = ",";
uint8_t resFailed[]  PROGMEM = "FAILED";
void DoExtraTasks(uint8_t dosend);


uint16_t GetAbsDelta(uint32_t orig, uint32_t mes)
{
    if (orig < mes)
        return GetAbsDelta(mes, orig);

    uint32_t diff = (orig-mes) * 10000;
    return (diff / orig);
}

/**
 * @brief SelfTestFull Full self testing without GPS
 * @return 0 - Ok, otherwise - number of failed tests
 *
 * 1. Check LOCK ping
 * 2. For 10 Mhz, 13.38 Mhz, 40.96 Mhz, 52 Mhz, 96 Mhz
 *  a. Set freqency
 *  b. Check lock state
 *  c. Count freqency using local oscillator
 */
static uint8_t SelfTestFull()
{
    FillResultNoNewLinePM(stPragma);
    FillResultNoNewLinePM(stLck);
    if (!SelfTestLockPin()) {
        FillResultPM(resFailed);
        return 0;
    }
    FillResultPM(resOk);

    const uint32_t freqs[] = {10000000, 13380000, 40960000, 49000000, 50000000, 51000000, 52000000, 53000000, 54000000, 55000000, 56000000, 96000000};
    const uint8_t accums[] = {2, 8, 32};

    uint8_t i;
    uint8_t failed = 0;

    for (i = 0; i < (sizeof(freqs)/sizeof(freqs[0])); i++) {
        DoExtraTasks(1);

        FillResultNoNewLinePM(stPragma);
        FillResultNoNewLinePM(stFreq);

        Fout = freqs[i];
        FillUint32(Fout);

        SetLMX2531(0);
        SetLMK();

        _delay_ms(250);

        if (IsVcoLocked()) {
            FillResultNoNewLinePM(stComma);
            FillResultPM(resOk);

            uint8_t j;
            // Compute frequency
            for (j = 0; j < (sizeof(accums)/sizeof(accums[0])); j++) {
                DoExtraTasks(1);

                SelfTestStart(accums[j]);
                uint8_t k;
                for (k = 0; k < accums[j]; k++) {
                    _delay_ms(97);
                    //DoExtraTasks(1);
                }
                SelfTestStop();
                FillResultNoNewLinePM(stPragma);
                FillResultNoNewLinePM(stCntd);
                FillUint16(accums[j]);
                FillResultNoNewLinePM(stComma);

#if 0
                DoExtraTasks(1);
                FillUint32(SelfMin);
                FillResultNoNewLinePM(stComma);
                FillUint32(SelfMax);
                FillResultNoNewLinePM(stComma);
#endif
                DoExtraTasks(1);

                uint32_t cmax = SelfToOutputFreq(SelfMax);
                uint32_t cmin = SelfToOutputFreq(SelfMin);

                FillUint32(cmin);
                FillResultNoNewLinePM(stComma);
                FillUint32(cmax);
                FillResultNoNewLinePM(stComma);
                //FillResultPM(stComma);

#if 0
                DoExtraTasks(1);
                FillUint16(GetAbsDelta(Fout, cmin));
                FillResultNoNewLinePM(stComma);
                FillUint16(GetAbsDelta(Fout, cmax));
                FillResultPM(stComma);
#endif
                uint16_t erra = GetAbsDelta(Fout, cmin);
                uint16_t errb = GetAbsDelta(Fout, cmax);
                uint32_t cerr = erra*erra + errb*errb;
                if (cerr < 1000) {
                    //SQRT error less than ~300ppm
                    FillResultPM(resOk);
                } else {
                    FillResultPM(resFailed);
                    failed++;
                }

                //CheckDelta
                // 1000*(Orig-Mesured)/Orig
            }

        } else {
            FillResultNoNewLinePM(stComma);
            FillResultPM(resFailed);

            failed++;
        }

        //Check lock status

    }

    return failed;
}

#endif


void UpdateOSCValue(void)
{
    int32_t delta = (int)(FilteredVal/FILTER_EXP_ALPHA) - (int)Fout ;

    uint32_t pdelta = (delta > 0) ? delta : -delta;
    if (pdelta < Fout / 4096)
    {
        //TODO Make pure div operation instead of this
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
    if ((CounterHHValue - LastAutoUpd > 500) && (Count1PPS > 80) && (ddd < 31) && (PPS_skipped == 0))
    {
        int32_t delta = (int)(FilteredVal/FILTER_EXP_ALPHA) - (int)Fout ;
        uint32_t pdelta = (delta > 0) ? delta : -delta;
        if (pdelta > 2)
        {
            UpdateOSCValue();

            PPS_skipped = RESET_OCP_COUNTER;

            LastAutoUpd = CounterHHValue;

            SetLMX2531(1);
        }

    }

}

#endif

extern uint8_t gpsmode;
extern uint8_t commands;
extern RingBuff_t USBtoUSART_Buffer;
extern RingBuff_t USARTtoUSB_Buffer;

ISR(SPI_STC_vect, ISR_BLOCK)
{
//	if (USB_DeviceState != DEVICE_STATE_Configured)
//	{
		if (USARTtoUSB_Buffer.Elements)
			SPDR = Buffer_GetElement(&USARTtoUSB_Buffer);
		else
			SPDR = 0xff;


	   	uint8_t byte = SPDR;
  	  	if (byte == '\n' || byte == '\r')
   		 	commands++;

		if ((byte != 0) && (byte != 0xff))
            Buffer_StoreElement(&USBtoUSART_Buffer, byte);
//	}
//	else
//	{
//		SPDR = 0xff;
//	}
	
}

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

#ifdef SELF_TESTING
    //Turn on digital VCO lock detection

#endif
}

void InitLMK(void)
{
    // Enable LMK01000
    LmkGoeSet();
    _delay_ms(50);

    LMK0X0XX_WRITE(0x80000100);
}





inline static void SetLMX2531_R7(uint16_t fosc)
{
#ifdef UNROLL_MACROSS_LMX2531

  uint16_t xtlman = (0x3ffc & ((((uint32_t)fosc)<<6)/(VCO_Kbit))) ;
  uint8_t xdiv = ( (fosc) < 40000 ? (((fosc) / 20000)+1) : 3);


  write_reg_LMX2531(xtlman >> 8, (uint8_t)xtlman | xdiv, 7);
#else
  LMX2531_WRITE( MAKE_R7(VCO_Kbit, fosc) );
#endif
}

inline static void SetLMX2531_R1_R0(uint32_t num, uint16_t n)
{
#ifdef UNROLL_MACROSS_LMX2531
    uint16_t numh = num>>12;
    uint8_t nh = n>>8;

    write_reg_LMX2531(0x20 | (ICP_1X<<1) | (nh>>2)  , (nh<<6) |  (numh >> 4) , (numh << 4) | 0x1);

    uint16_t nt = num;
    nt <<= 4;
    write_reg_LMX2531(n, nt >>8, (uint8_t)nt);

#else
    LMX2531_WRITE( MAKE_R1(ICP_1X, n >> 8, num >> 12) );
    LMX2531_WRITE( MAKE_R0(n & 0xFF, num & 0xFFF) );
#endif
}


uint8_t SetLMX2531(uint8_t tuneOnly)
{
    uint32_t num;

    uint8_t r = 1;
    if (Fosc > 14000000ul)
       r = 2;

    uint16_t vco;
    //uint16_t foutmhz = Fout / 1000000;
    uint16_t foutmhz = Fout / 100000;

    if (foutmhz == 0)
        return 0;

    uint16_t i;
#ifdef VCO_FIXED
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
#else

    uint8_t kk = 1;
    if (VCO_MAX > DIVIDER_MAX_FREQ)
    {
      kk++;
    }

    i = (((uint16_t)10*(VCO_MIN + VCO_MAX) >> (kk+1)) / (foutmhz)) << kk;
    vco = (i + 1<<kk) * (foutmhz);
    if (vco > 10*VCO_MAX)
      vco = i * (foutmhz);
    else
      i += 1<<kk;

#endif

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
    {
    uint32_t dev1;
    uint32_t rem1 = rem_n;
    uint8_t t;
    for (t = 0; t <= den_bit; t++)
    {
        if (t != 0)
        {
            rem1 <<= 1;
            num <<= 1;
        }

        dev1 = (rem1) / ((Fosc / r));
        rem1 = (rem1) % ((Fosc / r));

        if (t == 0)
            num = dev1;
        else if (dev1 & 1)
            num |= 1;

    }

    }
#endif

#ifdef VCO_FIXED
    if (VCO_MAX > DIVIDER_MAX_FREQ)
#else
    if (kk == 2)
#endif
        i /= 2;

    if (i > 510)
        return 0;

    if (!tuneOnly)
    {
        LMX2531_WRITE( MAKE_R8(LOCKMODE_LINEAR, Fosc/1000) );
        //LMX2531_WRITE( MAKE_R7(VCO_Kbit, Fosc/1000) );
        SetLMX2531_R7(Fosc/1000);

        LMX2531_WRITE( MAKE_R6(XTLSEL_MANUAL, VCO_ACI_SEL_M1, 1, R_40, R_10, R_40, R_10, C3_C4_100_100) );
        //LMX2531_WRITE( MAKE_R6(CALC_XTSEL(Fosc/1000000), VCO_ACI_SEL_M1, 1, R_40, R_10, R_40, R_10, C3_C4_100_100) );
        LMX2531_WRITE( MAKE_R4(ICP_1X, TOC_DISABLED) );
#ifdef DEBUG_REGS
        LMX2531_WRITE(tmp_r3 = MAKE_R3((VCO_MAX > DIVIDER_MAX_FREQ), FDM_FRACTIONAL, DITHER_STRONG, FRAC_ORDER_4, FOLD_VALUE, den >> 12) );
        LMX2531_WRITE(tmp_r2 = MAKE_R2(den & 0xFFF, r) );
    }
    LMX2531_WRITE(tmp_r1 = MAKE_R1(ICP_1X, n >> 8, num >> 12) );
    LMX2531_WRITE(tmp_r0 = MAKE_R0(n & 0xFF, num & 0xFFF) );
#else
        LMX2531_WRITE( MAKE_R3((VCO_MAX > DIVIDER_MAX_FREQ), FDM_FRACTIONAL, DITHER_STRONG, FRAC_ORDER_4, FOLD_VALUE, den >> 12) );
        LMX2531_WRITE( MAKE_R2(den & 0xFFF, r) );
    }
    //LMX2531_WRITE( MAKE_R1(ICP_1X, n >> 8, num >> 12) );
    //LMX2531_WRITE( MAKE_R0(n & 0xFF, num & 0xFFF) );

    SetLMX2531_R1_R0(num, n);
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
   uint8_t j = 1;
   for (uint8_t i = 0; i < 8; i++, j<<=1)
   {
        if ((j & LMK_OutMask) == j)
            //LMK0X0XX_WRITE(MAKE_LMK(1, 1, LMK_devider, 0, i));
            write_reg_LMK0X0XX(MAKE_LMK_HH(), MAKE_LMK_HL(1, 1), MAKE_LMK_LH(LMK_devider), MAKE_LMK_LL(0, i));
        else
            //LMK0X0XX_WRITE(0x00000100 | (uint32_t)i);
            write_reg_LMK0X0XX(MAKE_LMK_HH(), MAKE_LMK_HL(0, 0), MAKE_LMK_LH(1), MAKE_LMK_LL(0, i));
   }

#ifdef PRESENT_GPS
   LMK0X0XX_WRITE(MAKE_LMK(1, 1, GpsSync_divider * LMK_devider, 0, GPS_PORT));
#endif

   LMK0X0XX_WRITE(0x00022A09);
   LMK0X0XX_WRITE(0x6800000E);



   _delay_ms(10);

    LmkSyncClear();

   _delay_ms(10);

    LmkSyncSet();
}

#if TAMER_VER >= 12
#define ENABLE_OSC  PD6
#define DDR_OSC     DDRD
#define PORT_OSC    PORTD

void SetOscillatorMode(uint8_t mode)
{
    DDR_OSC |= (1 << ENABLE_OSC);

    if (mode)
    {
        PORT_OSC |= (1 << ENABLE_OSC);
    }
    else
    {
        PORT_OSC &= ~(1 << ENABLE_OSC);
    }
}

static void SetOscillator(void)
{
    SetOscillatorMode (EnableOscillator);
}

#endif

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

#if TAMER_VER >= 12
        SetOscillator();
#endif

        InitLMX2531();

        InitLMK();

        _delay_ms(50);

        if (SetLMX2531(0))
        {
            SetLMK();
        }
    }
#if TAMER_VER >= 12
    else
        SetOscillator();
#endif

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

uint8_t ProcessCommand(void)
{
    switch(command.cmd)
    {
        case cmdIDLE:
            return 1;

#if (TAMER_VER >= 122) && defined (PRESENT_GPS)
        case cmdGPSMODE:
    	    gpsmode = gpsmode ^ 1;
    	    return 1;
#endif

#ifndef NO_CMDREG
        case cmdREGISTER:
        {

            switch (command.type)
            {
                case trgLMK:
                    write_reg_LMK0X0XX(command.data[3], command.data[2], command.data[1], command.data[0]);
                    FillResultPM(resOk);
                    return 1;

                case trgLMX:
                    write_reg_LMX2531(command.data[2], command.data[1], command.data[0]);
                    FillResultPM(resOk);
                    return 1;
#ifdef PRESENT_DAC12
                case trgDAC:
                    switch (command.details)
                    {
                        case detD12:
                            write_reg_DAC12(command.data[1], command.data[0]);
                            return 1;
                        default:
                            return 0;
                    }
#endif
                default:
                    return 0;
            }
            break;
        }
#endif
#ifndef NO_CMDPIN
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
                            return 0;
                    }
                    FillResultPM(resOk);
                    return 1;
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
                            return 0;
                    }
                    FillResultPM(resOk);
                    return 1;
                }

                case trgLED:
                {
                    if (command.data[0])
                        LedSet();
                    else
                        LedClear();

                    FillResultPM(resOk);
                    return 1;
                }
                default:
                    return 0;
            }
         	return 0;
        }
#endif

        case cmdSET:
        {
            switch (command.type)
            {
                case trgNONE:
                {
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
                            return 0;
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
                    return 1;
                }
#if TAMER_VER >= 12
                case trgIOS:
                {
                    if (command.details == detEN)
                    {
                        if (command.data[0])
                            EnableOscillator = 1;
                        else
                            EnableOscillator = 0;

                        SetOscillator();
                        FillResultPM(resOk);
                        return 1;
                    }
                    return 0;
                }
#endif
                case trgVCO:
                    switch (command.details)
                    {
#ifndef VCO_FIXED
                        case detMIN:
                            VCO_MIN = command.u16data[0];
                            break;
                        case detMAX:
                            VCO_MAX = command.u16data[0];
                            break;
                        case detKBIT:
                            VCO_Kbit = command.u16data[0];
                            break;
#endif
                        default:
                            return 0;
                    }

                    FillResultPM(resOk);
                    return 1;

                case trgLMK:
                    switch (command.details)
                    {
                        case detPORTS:
                            LMK_OutMask = command.data[0];
                            break;

                        default:
                            return 0;
                    }

                    SetLMK();

                    FillResultPM(resOk);
                    return 1;
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
                            return 0;
                    }

                    if (SetDac(DacValue))
                        FillResultPM(resOk);
                    else
                        FillResultPM(resBadRange);
                    return 1;
#endif

#ifdef PRESENT_GPS
                case trgGPS:
                {
                    switch (command.details)
                    {
                        case detSYN:    UpdateOSCValue(); break;
                        case detAUTO:   AutoUpdateGps = command.data[0]; break;
                        default:
                            return 0;
                    }

                    FillResultPM(resOk);
                    return 1;
                }
#endif
#ifdef SELF_TESTING
                case trgSTS:
                {
                    switch (command.details)
                    {
                    case detSYN:
                        FillCmd();  FillUint16(SelfTestLockPin());      FillResultNoNewLinePM(newLine); return 1;
                    case detAUTO:
                        SelfTestFull(); return 1;
                    default:
                        if (command.data[0] == 0)
                            SelfTestStop();
                        else if (command.data[0] < 128)
                            SelfTestStart(command.data[0]);
                        else {
                            FillResultPM(resBadRange);
                            return 1;
                        }
                    }

                    FillResultPM(resOk);
                    return 1;
                }
#endif
                default:
                    return 0;
            }
            return 0;
        }
#ifndef NO_CMDINFO
        case cmdINFO:
        {
            switch (command.type)
            {
#if TAMER_VER >= 12
                case trgIOS:
                {
                    if (command.details == detEN)
                    {
                        FillCmd();  FillUint16(EnableOscillator); FillResultNoNewLinePM(newLine); return 1;
                    }
                    return 0;
                }
#endif
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
                        default: return 0;
                    }

                    return 1;
                }
#endif
                case trgNONE:
                {
                    switch (command.details)
                    {
                        case detOUT:   FillCmd();  FillUint32(Fout);      FillResultNoNewLinePM(newLine); break;
                        case detOSC:   FillCmd();  FillUint32(Fosc);      FillResultNoNewLinePM(newLine); break;
                        case detAUTO:  FillCmd();  FillUint16(AutoFreq);  FillResultNoNewLinePM(newLine); break;
                        default: return 0;
                    }
                    return 1;
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
#ifdef SELF_TESTING
                        case detLCK:   FillCmd();  FillUint32(IsVcoLocked() ? 1 : 0);     FillResultNoNewLinePM(newLine); break;
#endif //SELF_TESTING
                        default: return 0;
                    }
                    return 1;
                }

                case trgLMK:
                {
                    switch (command.details)
                    {
                        case detPORTS:    FillCmd();  FillUint16(LMK_OutMask);  FillResultNoNewLinePM(newLine); break;
                        case detDIVIDERS: FillCmd();  FillUint16(LMK_devider);  FillResultNoNewLinePM(newLine); break;
                        default: return 0;
                    }
                    return 1;
                }

#ifdef PRESENT_DAC12
                case trgDAC:  FillCmd();  FillUint16(DacValue);  FillResultNoNewLinePM(newLine); return 1;
#endif
#ifdef SELF_TESTING
                case trgSTS:
                {
                    switch (command.details)
                    {
                        case detMAX: FillCmd();   FillUint32(SelfMax);  FillResultNoNewLinePM(newLine); break;
                        case detMIN: FillCmd();   FillUint32(SelfMin);  FillResultNoNewLinePM(newLine); break;
                        case detR01: { int32_t tmp; cli(); tmp = SelfMax - SelfMin; sei(); FillCmd(); FillUint32(tmp);  FillResultNoNewLinePM(newLine); break; }

                        case detR02: FillCmd(); FillUint32(SelfToOutputFreq(SelfMax));  FillResultNoNewLinePM(newLine); break;
                        case detR03: FillCmd(); FillUint32(SelfToOutputFreq(SelfMin));  FillResultNoNewLinePM(newLine); break;

                        default: return 0;
                    }
                    return 1;
                }
#endif
                default:
                  return 0;
            }

            return 0;
        }
#endif
        case cmdRESET:
        {
            InitLMX2531();
            InitLMK();
            FillResultPM(resOk);
            return 1;
        }

#ifndef NO_HWINFO
        case cmdHWINFO:
            LoadHwInfo();
            return 1;
#endif

#ifndef NO_VERSION
        case cmdVERSION:
            FillResultPM(resVersion);
            return 1;
#endif

#ifndef NO_CMDEELOAD
        case cmdLOAD_EEPROM:
            LoadEEPROM();
            FillResultPM(resOk);
            return 1;
#endif

        case cmdSTORE_EEPROM:
            StoreEEPROM();
            FillResultPM(resOk);
            return 1;

        default:
            return 0;

    }
	return 0;
}
