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

#include <util/delay.h>

void write_reg_LMX2531(uint8_t f1, uint8_t f2, uint8_t f3)
{
        MicrowireWriteByte(f1);
        MicrowireWriteByte(f2);
        MicrowireWriteByte(f3);
        MicrowireLatchLMX2531();
}

void write_reg_LMK0X0XX(uint8_t f1, uint8_t f2, uint8_t f3, uint8_t f4)
{
        MicrowireWriteByte(f1);
        MicrowireWriteByte(f2);
        MicrowireWriteByte(f3);
        MicrowireWriteByte(f4);
        MicrowireLatchLMK0X0XX();
}

void MicrowireInit()
{
   MW_DDR |=  (1 << MW_DATA) | (1 << MW_CLK);
   MW_PORT &= ~(1 << MW_CLK);
   MW_PORT &= ~(1 << MW_DATA);
}

void MicrowireLatchLMX2531()
{
    MW_LE_PORT |= (1 << MW_LE_LMX2531);
    MW_LE_PORT |= (1 << MW_LE_LMX2531);
   //_delay_us(1);
    MW_LE_PORT &= ~(1 << MW_LE_LMX2531);
    MW_LE_PORT &= ~(1 << MW_LE_LMX2531);
   //_delay_us(1);
}

void MicrowireLatchLMK0X0XX()
{
    MW_LE_PORT |= (1 << MW_LE_LMK0X0XX);
    MW_LE_PORT |= (1 << MW_LE_LMK0X0XX);
   //_delay_us(1);
    MW_LE_PORT &= ~(1 << MW_LE_LMK0X0XX);
    MW_LE_PORT &= ~(1 << MW_LE_LMK0X0XX);
   //_delay_us(1);
}


void BoardInit()
{
   MicrowireInit();

   MW_LE_DDR |=  (1 << MW_LE_LMX2531) | (1 << MW_LE_LMK0X0XX);
   MW_LE_PORT &= ~((1 << MW_LE_LMX2531) | (1 << MW_LE_LMK0X0XX));

   LmxCeInit();
   LmxCeClear();

   LmkCeInit();
   LmkSyncClear();
   LmkGoeClear();

   LedInit();
   LedClear();
}

void MicrowireWriteByte(uint8_t data)
{
   uint8_t i;

   for (i = 8; i != 0; i--)
   {
      MW_PORT &=  ~(1 << MW_CLK);  //Toggle CLK low

      // Use this because of perfomance impact on an embedded device
      if ((data & 0x80) != 0)
         MW_PORT |=  (1 << MW_DATA);   //Set DATA bit
      else
         MW_PORT &=  ~(1 << MW_DATA);  //Reset DATA bit

      data <<= 1;

      MW_PORT |=  (1 << MW_CLK);  //Toggle CLK high
   }

   MW_PORT &=  ~(1 << MW_DATA);
   MW_PORT &=  ~(1 << MW_CLK);
}


void MicrowireWriteByteF(uint8_t data)
{
   uint8_t i;

   MW_PORT &=  ~(1 << MW_CLK);

   for (i = 8; i != 0; i--)
   {
      MW_PORT |=  (1 << MW_CLK);  //Toggle CLK high

      // Use this because of perfomance impact on an embedded device
      if ((data & 0x80) != 0)
         MW_PORT |=  (1 << MW_DATA);   //Set DATA bit
      else
         MW_PORT &=  ~(1 << MW_DATA);  //Reset DATA bit

      data <<= 1;

      MW_PORT &=  ~(1 << MW_CLK);  //Toggle CLK low
   }

   MW_PORT &=  ~(1 << MW_DATA);
}


void write_reg_DAC12(uint8_t f1, uint8_t f2)
{
        DacSyncClear();
        MicrowireWriteByteF(f1);
        MicrowireWriteByteF(f2);
        DacSyncSet();
}