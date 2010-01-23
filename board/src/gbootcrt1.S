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

/* if not defined, assume old version with underscores */
#ifndef __USER_LABEL_PREFIX__
#define __USER_LABEL_PREFIX__ _
#endif

#ifndef __REGISTER_PREFIX__
#define __REGISTER_PREFIX__
#endif

/* the assembler line separator (just in case it ever changes) */
#define _L $

#define CONCAT1(a, b) CONCAT2(a, b)
#define CONCAT2(a, b) a ## b

#define _U(x) CONCAT1(__USER_LABEL_PREFIX__, x)

#define _R(x) CONCAT1(__REGISTER_PREFIX__, x)

/* these should help to fix the "can't have function named r1()" bug
   which may require adding '%' in front of register names.  */

#define r0 _R(r0)
#define r1 _R(r1)
#define r2 _R(r2)
#define r3 _R(r3)
#define r4 _R(r4)
#define r5 _R(r5)
#define r6 _R(r6)
#define r7 _R(r7)
#define r8 _R(r8)
#define r9 _R(r9)
#define r10 _R(r10)
#define r11 _R(r11)
#define r12 _R(r12)
#define r13 _R(r13)
#define r14 _R(r14)
#define r15 _R(r15)
#define r16 _R(r16)
#define r17 _R(r17)
#define r18 _R(r18)
#define r19 _R(r19)
#define r20 _R(r20)
#define r21 _R(r21)
#define r22 _R(r22)
#define r23 _R(r23)
#define r24 _R(r24)
#define r25 _R(r25)
#define r26 _R(r26)
#define r27 _R(r27)
#define r28 _R(r28)
#define r29 _R(r29)
#define r30 _R(r30)
#define r31 _R(r31)

#ifndef __tmp_reg__
#define __tmp_reg__ r0
#endif

#ifndef __zero_reg__
#define __zero_reg__ r1
#endif

#if __AVR_MEGA__
  #define XJMP jmp
  #define XCALL call
#else
  #define XJMP rjmp
  #define XCALL rcall
#endif




/*
    FIXME!

    For unknown reason all vectors in .bootvectors is misaligned after compilation.
    The workaround is adding *nop* after rjmp command.

*/


    .macro  vector name
    .if (. - __bootvectors < ((3*_VECTORS_SIZE)/2))
    .weak   \name
    .set    \name, __bootbad_interrupt
    jmp    \name
    nop
    .endif
    .endm

    .section .bootvectors,"ax",@progbits
    .global __bootvectors
    .func   __bootvectors
__bootvectors:
    jmp    __bootinit
    nop
    vector  __bootvector_1
    vector  __bootvector_2
    vector  __bootvector_3
    vector  __bootvector_4
    vector  __bootvector_5
    vector  __bootvector_6
    vector  __bootvector_7
    vector  __bootvector_8
    vector  __bootvector_9
    vector  __bootvector_10
    vector  __bootvector_11
    vector  __bootvector_12
    vector  __bootvector_13
    vector  __bootvector_14
    vector  __bootvector_15
    vector  __bootvector_16
    vector  __bootvector_17
    vector  __bootvector_18
    vector  __bootvector_19
    vector  __bootvector_20
    vector  __bootvector_21
    vector  __bootvector_22
    vector  __bootvector_23
    vector  __bootvector_24
    vector  __bootvector_25
    vector  __bootvector_26
    vector  __bootvector_27
    vector  __bootvector_28
    vector  __bootvector_29
    vector  __bootvector_30
    vector  __bootvector_31
    vector  __bootvector_32
    vector  __bootvector_33
    vector  __bootvector_34
    vector  __bootvector_35
    vector  __bootvector_36
    vector  __bootvector_37
    vector  __bootvector_38
    vector  __bootvector_39
    vector  __bootvector_40
    vector  __bootvector_41
    vector  __bootvector_42
    vector  __bootvector_43
    vector  __bootvector_44
    vector  __bootvector_45
    vector  __bootvector_46
    vector  __bootvector_47
    vector  __bootvector_48
    vector  __bootvector_49
    vector  __bootvector_50
    vector  __bootvector_51
    vector  __bootvector_52
    vector  __bootvector_53
    vector  __bootvector_54
    vector  __bootvector_55
    vector  __bootvector_56
    vector  __bootvector_57
    vector  __bootvector_58
    vector  __bootvector_59
    vector  __bootvector_60
    vector  __bootvector_61
    vector  __bootvector_62
    vector  __bootvector_63
    vector  __bootvector_64
    vector  __bootvector_65
    vector  __bootvector_66
    vector  __bootvector_67
    vector  __bootvector_68
    vector  __bootvector_69
    vector  __bootvector_70
    vector  __bootvector_71
    vector  __bootvector_72
    vector  __bootvector_73
    vector  __bootvector_74
    vector  __bootvector_75
    vector  __bootvector_76
    vector  __bootvector_77
    vector  __bootvector_78
    vector  __bootvector_79
    vector  __bootvector_80
    vector  __bootvector_81
    vector  __bootvector_82
    vector  __bootvector_83
    vector  __bootvector_84
    vector  __bootvector_85
    vector  __bootvector_86
    vector  __bootvector_87
    vector  __bootvector_88
    vector  __bootvector_89
    vector  __bootvector_90
    vector  __bootvector_91
    vector  __bootvector_92
    vector  __bootvector_93
    vector  __bootvector_94
    vector  __bootvector_95
    vector  __bootvector_96
    vector  __bootvector_97
    vector  __bootvector_98
    vector  __bootvector_99
    vector  __bootvector_100
    vector  __bootvector_101
    vector  __bootvector_102
    vector  __bootvector_103
    vector  __bootvector_104
    vector  __bootvector_105
    vector  __bootvector_106
    vector  __bootvector_107
    vector  __bootvector_108
    vector  __bootvector_109
    vector  __bootvector_110
    vector  __bootvector_111
    vector  __bootvector_112
    vector  __bootvector_113
    vector  __bootvector_114
    vector  __bootvector_115
    vector  __bootvector_116
    vector  __bootvector_117
    vector  __bootvector_118
    vector  __bootvector_119
    vector  __bootvector_120
    vector  __bootvector_121
    vector  __bootvector_122
    vector  __bootvector_123
    vector  __bootvector_124
    vector  __bootvector_125
    .endfunc



    .section .bootloader,"ax",@progbits
    .global __bootbad_interrupt
    .func   __bootbad_interrupt
__bootbad_interrupt:
    .weak   __bootvector_default
    .set    __bootvector_default, __bootvectors
    XJMP    __bootvector_default
    .endfunc

    .section .bootinit0,"ax",@progbits
    .weak   __bootinit
;   .func   __bootinit
__bootinit:

    .weak   __stack

    /* By default, malloc() uses the current value of the stack pointer
       minus __malloc_margin as the highest available address.

       In some applications with external SRAM, the stack can be below
       the data section (in the internal SRAM - faster), and __heap_end
       should be set to the highest address available for malloc().  */
    .weak   __heap_end
    .set    __heap_end, 0

    .section .bootinit2,"ax",@progbits
    clr __zero_reg__
    out AVR_STATUS_ADDR, __zero_reg__
    ldi r28,lo8(__stack)
#ifdef _HAVE_AVR_STACK_POINTER_HI
    ldi r29,hi8(__stack)
    out AVR_STACK_POINTER_HI_ADDR, r29
#endif  /* _HAVE_AVR_STACK_POINTER_HI */
    out AVR_STACK_POINTER_LO_ADDR, r28

#ifdef __AVR_3_BYTE_PC__
    ldi r16, hh8(pm(__bootvectors))
    out _SFR_IO_ADDR(EIND), r16
#endif  /* __AVR_3_BYTE_PC__ */


   /* copy initial global data values from FLASH into RAM */
   .section .bootinit4,"ax",@progbits
   .global __do_copy_bootdata
__do_copy_bootdata:
   ldi         r17, hi8(__bootdata_end)
   ldi         r26, lo8(__bootdata_start)
   ldi         r27, hi8(__bootdata_start)
   ldi         r30, lo8(__bootdata_load_start)
   ldi         r31, hi8(__bootdata_load_start)
   ldi         r16, hh8(__bootdata_load_start)
#if BIG_CODE
    /* Only for >64K devices with RAMPZ, replaces the default code
       provided by libgcc.S which is only linked in if necessary.  */
#ifdef __AVR_ENHANCED__
    ldi r16, hh8(__bootdata_load_start)
#else
    ldi r16, hh8(__bootdata_load_start - 0x10000)
.L__do_copy_bootdata_carry:
    inc r16
#endif /* __AVR_ENHANCED__ */
    out AVR_RAMPZ_ADDR, r16
    rjmp       .L__do_copy_bootdata_start
.L__do_copy_bootdata_loop:
#ifdef __AVR_ENHANCED__
    elpm    r0, Z+
#else
    elpm
#endif /* __AVR_ENHANCED__ */
    st  X+, r0
#ifndef __AVR_ENHANCED__
    adiw    r30, 1
    brcs    .L__do_copy_bootdata_carry
#endif /* __AVR_ENHANCED__ */
#else
    rjmp       .L__do_copy_bootdata_start
.L__do_copy_bootdata_loop:
    lpm        r0, Z+
    st         X+, r0
#endif /* BIG_CODE */
.L__do_copy_bootdata_start:
    cpi        r26, lo8(__bootdata_end)
    cpc        r27, r17
    brne       .L__do_copy_bootdata_loop


    .set    __stack, RAMEND

    .section .bootinit9,"ax",@progbits
#ifdef __AVR_ASM_ONLY__
    XJMP    bootmain
#else   /* !__AVR_ASM_ONLY__ */
    XCALL   bootmain
    ;XJMP    exit
    cli
.L__exit: rjmp .L__exit
#endif  /* __AVR_ASM_ONLY__ */
;   .endfunc


