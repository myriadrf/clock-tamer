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

/** \file
 *
 *  Main source file for the Tamer project. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */

#include "Tamer.h"
#include "TamerControl.h"
#include <avr/power.h>

#include <stdio.h>

#define SPI_ENABLED


/** Circular buffer to hold data from the host before it is sent to the device via the serial port. */
RingBuff_t USBtoUSART_Buffer;

/** Circular buffer to hold data from the serial port before it is sent to the host. */
RingBuff_t USARTtoUSB_Buffer;

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config = 
			{
				.ControlInterfaceNumber         = 0,

				.DataINEndpointNumber           = CDC_TX_EPNUM,
				.DataINEndpointSize             = CDC_TXRX_EPSIZE,
				.DataINEndpointDoubleBank       = false,

				.DataOUTEndpointNumber          = CDC_RX_EPNUM,
				.DataOUTEndpointSize            = CDC_TXRX_EPSIZE,
				.DataOUTEndpointDoubleBank      = false,

				.NotificationEndpointNumber     = CDC_NOTIFICATION_EPNUM,
				.NotificationEndpointSize       = CDC_NOTIFICATION_EPSIZE,
				.NotificationEndpointDoubleBank = false,
			},
	};



/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */

void FillUint16(uint16_t val);
void FillResultPM(uint8_t* res);



uint8_t resSyntax[] PROGMEM = "SYNTAX ERROR";
uint8_t resErr[] PROGMEM = "CMD ERROR";


volatile uint8_t commands = 0;

#if TAMER_VER >= 12
void SetOscillatorMode(uint8_t);
#endif

int main(void)
{
#if TAMER_VER >= 12
    SetOscillatorMode(0);
#endif

    INFOLED_DDR |=  (1 << INFOLED);
    INFOLED_PORT |= (1 << INFOLED);

    SetupHardware();
    BoardInit();

    AutoStartControl();

	Buffer_Initialize(&USBtoUSART_Buffer);
	Buffer_Initialize(&USARTtoUSB_Buffer);


    //CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);

	for (;;)
	{
		for (uint8_t DataBytesRem = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface); DataBytesRem != 0; DataBytesRem--)
		{
			if (!(BUFF_STATICSIZE - USBtoUSART_Buffer.Elements))
			  break;

            uint8_t byte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);

            if ((byte != 0) && (byte != 0xff))
                Buffer_StoreElement(&USBtoUSART_Buffer, byte);


			// Uncomment this to enable echo on console
            // CDC_Device_SendByte(&VirtualSerial_CDC_Interface, byte);

            if (byte == '\n' || byte == '\r')
            {
                commands++;
                break;
            }
		}

        // Clean up buffer if it's full and there're no commands
        if ((commands == 0) && (!(BUFF_STATICSIZE - USBtoUSART_Buffer.Elements)))
              Buffer_GetElement(&USBtoUSART_Buffer);
		

        for (;commands>0;commands--)
        {
            uint8_t res = ParseCommand();
            if (res)
            {
                res = ProcessCommand();
				if (res == 0)
				{
					FillResultPM(resErr);
				}
            }
            else
            {
                FillResultPM(resSyntax);
            }
        }


		if (USB_DeviceState == DEVICE_STATE_Configured)
		{
			/* Read bytes from the USART receive buffer into the USB IN endpoint */
			while (USARTtoUSB_Buffer.Elements)
				CDC_Device_SendByte(&VirtualSerial_CDC_Interface, Buffer_GetElement(&USARTtoUSB_Buffer));
		}

        TamerControlAux();
		
		/* Load bytes from the USART transmit buffer into the USART */
		//if (USBtoUSART_Buffer.Elements)
        //  Serial_TxByte(Buffer_GetElement(&USBtoUSART_Buffer));

        //Serial_TxByte(CDC_Device_SendByte(&VirtualSerial_CDC_Interface, '\n'));
		
		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
	}
}



/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* Hardware Initialization */
//	Serial_Init(38400, false);

#if (!defined(FIXED_CONTROL_ENDPOINT_SIZE))
    USB_ControlEndpointSize = CDC_CONTROL_ENDPOINT_SIZE;
#endif

    USB_Init();

#ifdef SPI_ENABLED
	//Enable MISO
	DDRB = (1<<PB3);

	SPCR = (1<<SPIE) | (1<<SPE) | (1<<CPOL);
	//SPDR = 0xff;
#endif

}


DEFINE_USERTRAP()



/** Event handler for the library USB Unhandled Control Request event. */
#ifdef NO_BOOTSHARED
void EVENT_USB_Device_UnhandledControlRequest(void)
#else
TRAP(TR_USB_DEVICE_UNHANDLEDCONTROLREQUEST)
#endif
{
    CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

/** Event handler for the library USB Configuration Changed event. */
#ifdef NO_BOOTSHARED
void EVENT_USB_Device_ConfigurationChanged(void)
#else
TRAP(TR_USB_DEVICE_CONFIGURATIONCHANGED)
#endif
{
    if (!(CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface)))
        LedClear();
	else
		VirtualSerial_CDC_Interface.State.LineEncoding.BaudRateBPS = 9600;
}

/** Event handler for the library USB Connection event. */
#ifdef NO_BOOTSHARED
void EVENT_USB_Device_Connect(void)
#else
TRAP(TR_USB_DEVICE_CONNECT)
#endif
{
    LedSet();
	SPCR &=~(1<<SPE);
}

/** Event handler for the library USB Disconnection event. */
#ifdef NO_BOOTSHARED
void EVENT_USB_Device_Disconnect(void)
#else
TRAP(TR_USB_DEVICE_DISCONNECT)
#endif
{
    LedClear();
	SPCR |= (1<<SPE);
}

uint16_t CALLBACK_NONDFU_USB_GetDescriptor(const uint16_t wValue, const uint8_t wIndex, void** const DescriptorAddress);

uint16_t TRAP_NAME(TR_USB_GETDESCRIPTOR) (const uint16_t wValue, const uint8_t wIndex, void** const DescriptorAddress);

uint16_t TRAP_NAME(TR_USB_GETDESCRIPTOR) (const uint16_t wValue, const uint8_t wIndex, void** const DescriptorAddress)
{
    CALLBACK_NONDFU_USB_GetDescriptor(wValue, wIndex, DescriptorAddress);
}


/** ISR to manage the reception of data from the serial port, placing received bytes into a circular buffer
 *  for later transmission to the host.
 */
/*
ISR(USART1_RX_vect, ISR_BLOCK)
{

}
*/



