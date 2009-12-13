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

#ifndef _TAMMER_PROTOCOL_H_
#define _TAMMER_PROTOCOL_H_


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



typedef enum tammerCommandType
{
    cmdIDLE = 0,
    cmdREG,
    cmdPIN,
    cmdSET,
    cmdSAVE,
    cmdDEFAULTS,
    cmdINFO,
    cmdVERSION,
    cmdRST,
    cmdLDE,
    cmdSTE,
} CommandType_t;

typedef enum tammerTargetType
{
    trgNONE = 0,
    trgLMK,
    trgLMX,
    trgDAC,
    trgLED,
    trgVCO,

} TargetType_t;

typedef enum tammerTargetDetails
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
} TargetDetails_t;


typedef struct tammerCommad
{
    CommandType_t    cmd;
    TargetType_t     type;
    TargetDetails_t  details;
    union {
        uint8_t          data[4];
        uint16_t         u16data[2];
        uint32_t         u32data;
    };

} __attribute__ ((__packed__))  TammerCommand_t;


#endif //_TAMMER_PROTOCOL_H_
