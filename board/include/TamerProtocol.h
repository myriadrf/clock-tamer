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

#ifndef _TAMER_PROTOCOL_H_
#define _TAMER_PROTOCOL_H_


/*

Register command

PIN,LMK,EN,1
PIN,LMX,GO,1
PIN,LMX,SY,1

PIN,LED,,00                      PIN LED{xx}             8bit value

REG,LMK,,00234a0e                REG LMK{xx.xx.xx.xx}    32bit value
REG,LMX,,003403                  REG LMX{xx.xx.xx}       24bit value
REG,DAC,,0002                    REG DAC{xx.xx.xx}       16-24bit value

SET,,OSC,10000200
SET,,OUT,52000000

SVE

NFO

*/



typedef enum tamerCommandType
{
    cmdIDLE = 0,
    cmdREGISTER,
    cmdPIN,
    cmdSET,
    cmdSAVE,
    cmdDEFAULTS,
    cmdINFO,
    cmdVERSION,
    cmdRESET,
    cmdLOAD_EEPROM,
    cmdSTORE_EEPROM,
} CommandType_t;

typedef enum tamerTargetType
{
    trgNONE = 0,
    trgLMK,
    trgLMX,
    trgDAC,
    trgLED,
    trgVCO,
    trgGPS,

} TargetType_t;

typedef enum tamerTargetDetails
{
    detNONE = 0,
    detEN,
    detGOE,
    detSYN,
    detOSC,
    detOUT,
    detPORTS,
    detMIN,
    detMAX,
    detKBIT,
    detDIVIDERS,
    detAUTO,
    detD12,
    detR00,
    detR01,
    detR02,
    detR03,
} TargetDetails_t;


typedef struct tamerCommad
{
    CommandType_t    cmd;
    TargetType_t     type;
    TargetDetails_t  details;
    union {
        uint8_t          data[4];
        uint16_t         u16data[2];
        uint32_t         u32data;
    };

} __attribute__ ((__packed__))  TamerCommand_t;


#endif //_TAMER_PROTOCOL_H_
