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

// gcc TammerProtocol.c -g -Wall -o TammerTest -D_SELF_TEST_


#include <stdint.h>
#ifndef _SELF_TEST_
#include "Tamer.h"
#else
#include <stdio.h>
#include <string.h>
#endif
#include "TamerProtocol.h"


#ifdef _SELF_TEST_

#define PROGMEM
uint8_t getbyte(void);
uint8_t pgm_read_byte(uint8_t* p);

#else

extern RingBuff_t USBtoUSART_Buffer;

#define getbyte()   Buffer_GetElement(&USBtoUSART_Buffer)

#endif


#define WORD_SIZE       3

uint8_t pCmd[]        PROGMEM  = "REG"
                                 "PIN"
                                 "SET"
                                 "SAV"
                                 "DEF"
                                 "NFO"
                                 "VER"
                                 "RST"
                                 "LDE"
                                 "STE";
#define CMD_COUNT    (sizeof(pCmd)  / WORD_SIZE)

uint8_t pTrg[]        PROGMEM  = "LMK"
                                 "LMX"
                                 "DAC"
                                 "LED"
                                 "VCO";
#define TRG_COUNT    (sizeof(pTrg)  / WORD_SIZE)

uint8_t pDet[]        PROGMEM  = "ENB"
                                 "GOE"
                                 "SYN"
                                 "OSC"
                                 "OUT"
                                 "PRT"
                                 "MIN"
                                 "MAX"
                                 "KBT"
                                 "DIV"
                                 "AUT";
#define DET_COUNT    (sizeof(pDet)  / WORD_SIZE)


TammerCommand_t command;

static inline uint8_t ParseParam(uint8_t w1, uint8_t w2, uint8_t w3, uint8_t* table)
{
    uint8_t i = 1;

    for (;;i++)
    {
        uint8_t o1 = pgm_read_byte(table++);
        uint8_t o2 = pgm_read_byte(table++);
        uint8_t o3 = pgm_read_byte(table++);

        if ((o1 == 0) /*|| (o2 == 0) || (o3 == 0)*/)
            return 0;

        if ((o1 == w1) && (o2 == w2) && (o3 == w3))
            return i;
    }
}

static uint8_t ParseValueD(uint8_t w1)
{
    if (w1 >= '0' && w1 <= '9')
        return w1 - '0';

    return 0x10;
}
static uint8_t ParseValue(uint8_t w1)
{
    if (w1 >= '0' && w1 <= '9')
        return w1 - '0';
    if (w1 >= 'a' && w1 <= 'f')
        return 10 + w1 - 'a';
    if (w1 >= 'A' && w1 <= 'F')
        return 10 + w1 - 'A';

    return 0x10;
}

uint8_t ParseCommand(void)
{
    uint8_t byte;
    uint8_t byte2;
    uint8_t byte3;

    uint8_t step = 0;
    uint8_t val = 0;


    command.cmd = 0;
    command.type = 0;
    command.details = 0;
    command.u32data = 0;

    for (; step < 4; )
    {
        // Skip white spaces
        do
        {
            byte = getbyte();
        } while (byte == ' ');

        if (byte == ',')
        {
            step++;
            val = 0;
            continue;
        }

        if (byte == 0 || byte == '\r' || byte == '\n')
            return 1;


        if (step < 3)
        {
            byte2 = getbyte();
            byte3 = getbyte();

            uint8_t* mem;
            switch (step)
            {
                case 0: mem = pCmd; break;
                case 1: mem = pTrg; break;
                case 2: mem = pDet; break;
            }
            val = ParseParam(byte, byte2, byte3, mem);
            switch (step)
            {
                case 0: command.cmd = val;     break;
                case 1: command.type = val;    break;
                case 2: command.details = val; break;
            }
        }
        else
        {
            uint8_t b1;
            uint8_t j;

            if ((byte == 'x') || (byte == 'X'))
            {

              for (j = 0; j < 8; j++)
              {
                b1 = ParseValue((byte = getbyte()));
                if (b1 == 0x10)
                    goto skip;

                command.u32data = (command.u32data << 4) | b1;
              }
            }
            else
            {
              b1 = ParseValueD(byte);
              if (b1 == 0x10)
                    goto skip;
              command.u32data = b1;

              for (j = 1; j < 10; j++)
              {
                b1 = ParseValueD((byte = getbyte()));
                if (b1 == 0x10)
                    goto skip;

                command.u32data = (command.u32data * 10) + b1;
              }
            }
            byte = getbyte();
        skip:
            if (byte == 0 || byte == '\r' || byte == '\n')
                return 1;
            return 0;
         }
    }

    return 0;
}



#ifdef _SELF_TEST_


uint8_t* buffer;
int pos = 0;
int len;

uint8_t getbyte(void)
{
    if (pos < len)
    {
        printf("@%i", pos);
        return buffer[pos++];
    }

    return 0;
}

uint8_t pgm_read_byte(uint8_t* p)
{
    return *p;
}


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("usage %s string\n", argv[0]);
        return -1;
    }

    buffer = (uint8_t*)argv[1];
    len = strlen(argv[1]);

    printf(" Command \"%s\", len = %d\n", buffer, len);
    int j = 0;

    for (; j < 10; j++)
    {
        int i = ParseCommand();
        printf(" Res = %d \n", i);

        printf(" CMD     %x\n", command.cmd);
        printf(" TYPE    %x\n", command.type);
        printf(" DETAILS %x\n", command.details);
        printf(" DATA    %02x.%02x.%02x.%02x  [%u.%u ; %u]\n", command.data[0], command.data[1], command.data[2], command.data[3],
            command.u16data[0], command.u16data[1], command.u32data);

        if (i == 0)
          return 0;
    }
    printf("Interrupted!\n");
    return 0;
}


#endif


