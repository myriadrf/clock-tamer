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

#if (__GNUC__ < 3) || (__GNUC__ == 3 && __GNUC_MINOR__ < 3)
#error "GCC version >= 3.3 required"
#endif


#include <avr/io.h>

#if __AVR_MEGA__
  #define XJMP jmp
  #define XCALL call
#else
  #define XJMP rjmp
  #define XCALL rcall
#endif


#define TRAPS_COUNT           12

    .macro bootstrap name
;    .if (__bootstraps_end - .  < TRAPS_COUNT)
    .weak   \name
    .set    \name, __bootstraps_none
    .balignl
     XJMP    \name
     nop
;    .endif
    .endm


    .section .bootstrap,"ax",@progbits
    .global __bootstraps_start
    .global __bootstraps_end
__bootstraps_end:
__bootstraps_vec11:
    bootstrap __bootstrap11
__bootstraps_vec10:
    bootstrap __bootstrap10
__bootstraps_vec9:
    bootstrap __bootstrap9
__bootstraps_vec8:
    bootstrap __bootstrap8
__bootstraps_vec7:
    bootstrap __bootstrap7
__bootstraps_vec6:
    bootstrap __bootstrap6
__bootstraps_vec5:
    bootstrap __bootstrap5
__bootstraps_vec4:
    bootstrap __bootstrap4
__bootstraps_vec3:
    bootstrap __bootstrap3
__bootstraps_vec2:
    bootstrap __bootstrap2
__bootstraps_vec1:
    bootstrap __bootstrap1
__bootstraps_vec0:
__bootstraps_start:
    bootstrap __bootstrap0

    .section .init7,"ax",@progbits
    jmp .Lskip_trap
    .global __bootstraps_none
    .func   __bootstraps_none
__bootstraps_none:
    ret
.Lskip_trap:
    .endfunc
