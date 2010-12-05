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

PIN,LMK,ENB,1
PIN,LMX,GOE,1
PIN,LMX,SYN,1

PIN,LED,,00                       PIN LED{xx}             8bit value

REG,LMK,,x00234a0e                REG LMK{xx.xx.xx.xx}    32bit value
REG,LMX,,x003403                  REG LMX{xx.xx.xx}       24bit value
REG,DAC,,x0002                    REG DAC{xx.xx.xx}       16-24bit value

SET,,OSC,10000200
SET,,OUT,52000000

SVE

NFO

*/


uint8_t IsCommandSeparator(uint8_t byte);

/** @brief Internal values of control commands classes.
*
*  @see This enum is tightly coupled with \p pCmd variable in
*       TamerProtocol.c source file, which contains string
*       representation of the commands.
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
    cmdHWINFO,
    cmdRESET,
    cmdLOAD_EEPROM,
    cmdSTORE_EEPROM,
} CommandType_t;

/** @brief Internal values of control commands target types.
*
*  @see This enum is tightly coupled with \p pTrg variable in
*       TamerProtocol.c source file, which contains string
*       representation of the commands.
*/
typedef enum tamerTargetType
{
    trgNONE = 0,
    trgLMK,
    trgLMX,
    trgDAC,
    trgLED,
    trgVCO,
    trgGPS,
    trgIOS

} TargetType_t;

/** @brief Internal values of control commands target details.
*
*  @see This enum is tightly coupled with \p pDet variable in
*       TamerProtocol.c source file, which contains string
*       representation of the commands.
*/
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

/** @brief Internal (decoded) command representation.
*/
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
