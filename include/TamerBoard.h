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

#ifndef _TAMMER_BOARD_H_
#define _TAMMER_BOARD_H_


#include <avr/io.h>


#define MW_DATA           PD0
#define MW_CLK            PD1

#define MW_DDR            DDRD
#define MW_PORT           PORTD


#define LMX_CE            PD5
#define LMX_CE_DDR        DDRD
#define LMX_CE_PORT       PORTD

#define LMK_SYNC          PC2
#define LMK_GOE           PC6
#define LMK_CE_DDR        DDRC
#define LMK_CE_PORT       PORTC


#define MW_LE_LMX2531     PB6
#define MW_LE_LMK0X0XX    PB7
#define MW_LE_DDR         DDRB
#define MW_LE_PORT        PORTB


#define INFOLED           PB5
#define INFOLED_DDR       DDRB
#define INFOLED_PORT      PORTB


#define PIN_ENTBLOADER    PB0

void MicrowireInit(void);
void MicrowireWriteByte(uint8_t data);



void MicrowireLatchLMX2531(void);
void MicrowireLatchLMK0X0XX(void);


void BoardInit(void);


#define LMX2531_WRITE(x)  write_reg_LMX2531( \
  (uint8_t)((x) >> 16), \
  (uint8_t)((x) >> 8), \
  (uint8_t)((x)))

#define LMK0X0XX_WRITE(x)  write_reg_LMK0X0XX( \
  (uint8_t)((x) >> 24), \
  (uint8_t)((x) >> 16), \
  (uint8_t)((x) >> 8), \
  (uint8_t)((x)))


void write_reg_LMX2531(uint8_t f1, uint8_t f2, uint8_t f3);
void write_reg_LMK0X0XX(uint8_t f1, uint8_t f2, uint8_t f3, uint8_t f4);


#define LedInit()          INFOLED_DDR |=  (1 << INFOLED)
#define LedSet()           INFOLED_PORT |= (1 << INFOLED)
#define LedClear()         INFOLED_PORT &= ~(1 << INFOLED)

#define LmxCeInit()        LMX_CE_DDR |=  (1 << LMX_CE)
#define LmxCeSet()         LMX_CE_PORT |= (1 << LMX_CE)
#define LmxCeClear()       LMX_CE_PORT &= ~(1 << LMX_CE)

#define LmkCeInit()        LMK_CE_DDR |=  ((1 << LMK_SYNC) | (1 << LMK_GOE))
#define LmkSyncSet()       LMK_CE_PORT |= (1 << LMK_SYNC)
#define LmkSyncClear()     LMK_CE_PORT &= ~(1 << LMK_SYNC)
#define LmkGoeSet()        LMK_CE_PORT |= (1 << LMK_GOE)
#define LmkGoeClear()      LMK_CE_PORT &= ~(1 << LMK_GOE)



#endif //_TAMMER_BOARD_H_
