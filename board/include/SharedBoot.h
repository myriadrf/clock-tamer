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
 *  Header file for SharedBoot.h.
 */

#ifndef _SHARED_BOOT_H_
#define _SHARED_BOOT_H_

#define CONCAT2(x,y)      x ## y
#define CONCAT(x,y)       CONCAT2(x,y)
#define TRAP_NAME(x)      CONCAT(__bootstrap, x)

#define TRAP(x)           void TRAP_NAME(x) (void)


#define STRINGIFY2(x)     #x
#define STRINGIFY(x)      STRINGIFY2(x)


/** Trap for running the user code */
#define TR_USERCODE                                 0

/** Trap for CALLBACK_USB_GetDescriptor in user code */
#define TR_USB_GETDESCRIPTOR                        1

/** Trap for EVENT_USB_Device_Connect in user code */
#define TR_USB_DEVICE_CONNECT                       2

/** Trap for EVENT_USB_Device_Disconnect in user code */
#define TR_USB_DEVICE_DISCONNECT                    3

/** Trap for EVENT_USB_Device_UnhandledControlRequest in user code */
#define TR_USB_DEVICE_UNHANDLEDCONTROLREQUEST       4

/** Trap for EVENT_USB_Device_ConfigurationChanged in user code */
#define TR_USB_DEVICE_CONFIGURATIONCHANGED          5

/** Trap for EVENT_USB_Device_Suspend in user code */
#define TR_USB_DEVICE_SUSPEND                       6

/** Trap for EVENT_USB_Device_WakeUp in user code */
#define TR_USB_DEVICE_WAKEUP                        7

/** Trap for EVENT_USB_Device_Reset in user code */
#define TR_USB_DEVICE_RESET                         8

/** Trap for EVENT_USB_Device_StartOfFrame in user code */
#define TR_USB_DEVICE_STARTOFFRAME                  9


#define JMP_TRAP(x)               __asm__ __volatile("jmp __bootstraps_start - 4*"  STRINGIFY(x) ";\r\n")
#define CALL_TRAP(x)              __asm__ __volatile("call __bootstraps_start - 4*" STRINGIFY(x) ";\r\n")
#define DEFINE_USERTRAP()         __attribute((naked)) TRAP(TR_USERCODE) { __asm__ __volatile("jmp 0x0000;\r\n"); }


//#define LUFA_SHARED_DATA_SECTION __attribute__ ((section (".shareddata")))

#endif //_SHARED_BOOT_H_
