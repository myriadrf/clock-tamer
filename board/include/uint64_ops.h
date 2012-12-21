/*
 * uint64_t calculation routines
 *
 * (c) 2010 Matthias Hopf <mat@mshopf.de>
 * License: GPL v2
 */

/*
 * Include either
 *   - one C file with
 *       #define BIND static
 *     to define all functions static
 *     (optimal for one C file)
 * or
 *   - one file with
 *       #undef BIND extern
 *     all others with
 *       #undef BIND
 *     to define all functions extern except some inlines
 *     (optimal for multiple C files)
 */

#ifndef _UINT64_OPS_H_
#define _UINT64_OPS_H_


#define USE_C		0		// 1 to not use ASM variants


#ifndef BIND
#  define BINDDECL extern
#else
#  define BINDDECL BIND
#endif


/* General issue about 64bit assembler: gcc's asm() doesn't allow to allocate
 * 64bit registers (%[EFGH]0 is replaced by %A0 instead of the registers for
 * the upper 32bit).
 * Thus gcc's calling convention must be used, and according routines marked as
 * non-inline (in order to really get the parameters in these registers).
 * The result is pushed back in the first (64bit) argument of the function,
 * as the return value happens to be stored in the same registers as the first
 * argument.
 * In order to have gcc return the right value, the registers of the first
 * argument 'a' are NOT clobbered, tricking gcc into believing that the
 * argument 'a' is still in these registers, and it won't do anything when
 * 'return a;' is executed.
 * Yes, this is a bit fragile. Not too bad, though. */


/* Shift right by single bit */
BINDDECL uint64_t uint64_sr (uint64_t v) __attribute ((noinline));
#ifdef BIND
BINDDECL uint64_t uint64_sr (uint64_t v)
{
# if USE_C
    return v >> 1;
# else
    /* If 64bit register refs with asm() worked this could be made inline:
     * asm volatile (" lsr %H0\n ror %G0\n ror %F0\n ror %E0\n"
     *               " ror %D0\n ror %C0\n ror %B0\n ror %A0\n"
     *               : "=r" (v) : "0"); */
    /* in:   v 64bit: r18-r25  replaced by out
     * out:    64bit: r18-r25  */
    /* This saves 200 bytes compared to the C variant, and many cycles. */
    asm volatile ("\n"
		  " lsr r25\n"
		  " ror r24\n"
		  " ror r23\n"
		  " ror r22\n"
		  " ror r21\n"
		  " ror r20\n"
		  " ror r19\n"
		  " ror r18\n");
    return v;
# endif
}
#endif

/* Shift left by single bit */
BINDDECL uint64_t uint64_sl (uint64_t v) __attribute ((noinline));
#ifdef BIND
BINDDECL uint64_t uint64_sl (uint64_t v)
{
# if USE_C
    return v << 1;
# else
    /* in:   v 64bit: r18-r25  replaced by out
     * out:    64bit: r18-r25  */
    /* This saves 200 bytes compared to the C variant, and many cycles. */
    asm volatile ("\n"
		  " lsl r18\n"
		  " rol r19\n"
		  " rol r20\n"
		  " rol r21\n"
		  " rol r22\n"
		  " rol r23\n"
		  " rol r24\n"
		  " rol r25\n");
    return v;
# endif
}
#endif

/* Shift right by count bits */
static inline uint64_t uint64_shiftr (uint64_t v, uint8_t count)
{
    while (count--)
	v = uint64_sr (v);
    return v;
}

/* Shift left by count bits */
static inline uint64_t uint64_shiftl (uint64_t v, uint8_t count)
{
    while (count--)
	v = uint64_sl (v);
    return v;
}

BINDDECL uint64_t uint64_mul32 (uint64_t a, uint32_t b) __attribute ((noinline));
#ifdef BIND
BINDDECL uint64_t uint64_mul32 (uint64_t a, uint32_t b)
{
# if USE_C
    uint64_t r = 0;
    while (b) {
	if ((uint8_t)b & 1)
	    r += a;
	a = uint64_sl (a);
	b >>= 1;
    }
    return r;
# else
    /* in:   a 64bit: r18-r25  replaced by out
     * in:   b 32bit: r14-r17  clobbered
     * tmp:  r 64bit: r6 -r13  clobbered
     * out:    64bit: r18-r25  */
    /* This saves 212 bytes compared to the C variant, and many cycles. */
    asm volatile (" clr r6\n"		// r = 0
		  " clr r7\n"
		  " clr r8\n"
		  " clr r9\n"
		  " clr r10\n"
		  " clr r11\n"
		  " clr r12\n"
		  " clr r13\n"
		  "0:\n"		// loop
		  " cp  r14, __zero_reg__\n" // b == 0 ? -> end
		  " cpc r15, __zero_reg__\n"
		  " cpc r16, __zero_reg__\n"
		  " cpc r17, __zero_reg__\n"
		  " breq 2f\n"
		  " lsr r17\n"		// b >> 1
		  " ror r16\n"
		  " ror r15\n"
		  " ror r14\n"
		  " brcc 1f\n"		// b[no underflow] -> no addition
		  " add r6, r18\n"	// r += a
		  " adc r7, r19\n"
		  " adc r8, r20\n"
		  " adc r9, r21\n"
		  " adc r10, r22\n"
		  " adc r11, r23\n"
		  " adc r12, r24\n"
		  " adc r13, r25\n"
		  "1:\n"
		  " lsl r18\n"		// a << 1
		  " rol r19\n"
		  " rol r20\n"
		  " rol r21\n"
		  " rol r22\n"
		  " rol r23\n"
		  " rol r24\n"
		  " rol r25\n"
		  " rjmp 0b\n"
		  "2:\n"		// end
		  " movw r18, r6\n"	// a = r
		  " movw r20, r8\n"
		  " movw r22, r10\n"
		  " movw r24, r12\n"
		 : : :
		 "r6","r7","r8","r9","r10","r11","r12","r13",
		 "r14","r15","r16","r17");
    return a;
# endif
}
#endif

BINDDECL uint64_t uint64_div32 (uint64_t a, uint32_t b) __attribute ((noinline));
#ifdef BIND
BINDDECL uint64_t uint64_div32 (uint64_t a, uint32_t b)
{
# if USE_C
    uint64_t r = 0;
    uint32_t h = 0;
    uint8_t c = 64, h2;
    /* This looks much smoother in assembler (carry)... */
    while (c--) {
	h2 = (h & 0x80000000) ? 1 : 0;
	h <<= 1;
	if (a & 0x8000000000000000ULL)
	    h |= 1;
	a = uint64_sl (a);
	r = uint64_sl (r);
	if (h2 || h >= b) {
	    h -= b;
	    r |= 1;
	}
    }
    return r;
# else
    /* This requires almost all registers; only r0/r1 and r26-r31 remain free.
     * in:   a 64bit: r18-r25  replaced by out
     * in:   b 32bit: r14-r17  unchanged
     * tmp:  r 64bit: r6 -r13  clobbered
     * tmp:  h 32bit: r2 -r5   clobbered
     * out:    64bit: r18-r25  */
    /* Based on 16:8 bit divide source from: http://www.avr-asm-tutorial.net */
    /* The routine uses the highest bit in 'r' for counting up to 64 - it's
     * shift left out of the register after 64 iterations. */
    /* This saves 174 bytes compared to the C variant, and many cycles. */
    asm volatile (" clr r2\n"		// h = 0
		  " clr r3\n"
		  " clr r4\n"
		  " clr r5\n"
		  " clr r6\n"		// r = 1
		  " inc r6\n"
		  " clr r7\n"
		  " clr r8\n"
		  " clr r9\n"
		  " clr r10\n"
		  " clr r11\n"
		  " clr r12\n"
		  " clr r13\n"
		  "0:\n"		// loop
		  " lsl r18\n"		// a << 1
		  " rol r19\n"
		  " rol r20\n"
		  " rol r21\n"
		  " rol r22\n"
		  " rol r23\n"
		  " rol r24\n"
		  " rol r25\n"
		  " rol r2\n"		// h << 1 | a[overflow]  (extends a to 96bit h:a)
		  " rol r3\n"
		  " rol r4\n"
		  " rol r5\n"
		  " brcs 1f\n"		// h[overflow] ? -> definitely h > b
		  " cp  r2, r14\n"	// h < b       ?
		  " cpc r3, r15\n"
		  " cpc r4, r16\n"
		  " cpc r5, r17\n"
		  " brcs 2f\n"		//               -> no subtraction
		  "1:\n"
		  " sub r2, r14\n"	// h -= b
		  " sbc r3, r15\n"	// if h < b  h overflowed before -> correct result
		  " sbc r4, r16\n"
		  " sbc r5, r17\n"
		  " sec\n"		// c = 1
		  " rjmp 3f\n"		// continue with r shift
		  "2:\n"
		  " clc\n"		// c = 0
		  "3:\n"
		  " rol r6\n"		// r = (r << 1) | c
		  " rol r7\n"
		  " rol r8\n"
		  " rol r9\n"
		  " rol r10\n"
		  " rol r11\n"
		  " rol r12\n"
		  " rol r13\n"
		  " brcc 0b\n"		// r[no overflow] ? -> loop
		  " movw r18, r6\n"	// a = r
		  " movw r20, r8\n"
		  " movw r22, r10\n"
		  " movw r24, r12\n"
		 : : :
		 "r2","r3","r4","r5",
		 "r6","r7","r8","r9","r10","r11","r12","r13");
    return a;
# endif
}
#endif


#endif

