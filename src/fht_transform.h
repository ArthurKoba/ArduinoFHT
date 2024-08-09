#ifndef AVR_FHT_FHT_TRANSFORM_H
#define AVR_FHT_FHT_TRANSFORM_H

static inline void fht_transform() {
    // save registers that are getting clobbered
    // avr-gcc requires r2:r17,r28:r29, and r1 cleared
    asm volatile (
        "push r2 \n"
        "push r3 \n"
        "push r4 \n"
        "push r5 \n"
        "push r6 \n"
        "push r7 \n"
        "push r8 \n"
        "push r9 \n"
        "push r10 \n"
        "push r11 \n"
        "push r12 \n"
        "push r13 \n"
        "push r14 \n"
        "push r15 \n"
        "push r16 \n"
        "push r17 \n"
        "push r28 \n"
        "push r29 \n"
    );

    // do first 3 butterflies - only 1 multiply, minimizes data fetches
    // initialize
    asm volatile (
        "clr r15 \n" // clear the null register
        "ldi r16, " STRINGIFY(FHT_SAMPLES_N/8) " \n" // prep loop counter
        "mov r14,r16 \n"
        "ldi r28, lo8(fht_input) \n" //set to beginning of data space
        "ldi r29, hi8(fht_input) \n"

        // first set: x0 and x1
        "1: \n"
        "ld r6,y \n" // fetch x0
        "ldd r7,y+1 \n"
        "ldd r4,y+2 \n" // fetch x1
        "ldd r5,y+3 \n"
        "asr r7 \n" // divide by 2 to keep from overflowing
        "ror r6 \n"
        "movw r8,r6 \n" // make backup
        "asr r5 \n" // divide by 2 to keep from overflowing
        "ror r4 \n"
        "add r6,r4 \n" // x0 + x1
        "adc r7,r5 \n"
        "sub r8,r4 \n" // x0 - x1
        "sbc r9,r5 \n"

        // first set: x2 and x3
        "ldd r10,y+4 \n" // fetch x2
        "ldd r11,y+5 \n"
        "ldd r4,y+6 \n" // fetch x3
        "ldd r5,y+7 \n"
        "asr r11 \n" // divide by 2 to keep from overflowing
        "ror r10 \n"
        "movw r12,r10 \n" // make backup
        "asr r5 \n" // divide by 2 to keep from overflowing
        "ror r4 \n"
        "add r10,r4 \n" // x2 + x3
        "adc r11,r5 \n"
        "sub r12,r4 \n" // x2 - x3
        "sbc r13,r5 \n"

        // first set: x4 and x5
        "ldd r22,y+8 \n" // fetch x4
        "ldd r23,y+9 \n"
        "ldd r4,y+10 \n" // fetch x5
        "ldd r5,y+11 \n"
        "asr r23 \n" // divide by 2 to keep from overflowing
        "ror r22 \n"
        "movw r16,r22 \n" // make backup
        "asr r5 \n" // divide by 2 to keep from overflowing
        "ror r4 \n"
        "add r22,r4 \n" // x4 + x5
        "adc r23,r5 \n"
        "sub r16,r4 \n" // x4 - x5
        "sbc r17,r5 \n"

        // first set: x6 and x7
        "ldd r24,y+12 \n" // fetch x6
        "ldd r25,y+13 \n"
        "ldd r4,y+14 \n" // fetch x7
        "ldd r5,y+15 \n"
        "asr r25 \n" // divide by 2 to keep from overflowing
        "ror r24 \n"
        "movw r18,r24 \n" // make backup
        "asr r5 \n" // divide by 2 to keep from overflowing
        "ror r4 \n"
        "add r24,r4 \n" // x6 + x7
        "adc r25,r5 \n"
        "sub r18,r4 \n" // x6 - x7
        "sbc r19,r5 \n"

        // second set: x0, x1, x2, and x3
        "asr r7 \n" // divide by 2 to keep from overflowing
        "ror r6 \n"
        "movw r4,r6 \n" // make backup
        "asr r11 \n" // divide by 2 to keep from overflowing
        "ror r10 \n"
        "add r6,r10 \n" // (x0 + x1) +  (x2 + x3)
        "adc r7,r11 \n"
        "sub r4,r10 \n" // (x0 + x1) - (x2 + x3)
        "sbc r5,r11 \n"
        "asr r9 \n" // divide by 2 to keep from overflowing
        "ror r8 \n"
        "movw r10,r8 \n" // make backup
        "asr r13 \n" // divide by 2 to keep from overflowing
        "ror r12 \n"
        "add r8,r12 \n" // (x0 - x1) +  (x2 - x3)
        "adc r9,r13 \n"
        "sub r10,r12 \n" // (x0 - x1) - (x2 - x3)
        "sbc r11,r13 \n"

        // second set: x4, x5, x6, and x7
        // negatives arent done as they cancel out in the next step
        "asr r23 \n" // divide by 2 to keep from overflowing
        "ror r22 \n"
        "movw r12,r22 \n" // make backup
        "asr r25 \n" // divide by 2 to keep from overflowing
        "ror r24 \n"
        "add r22,r24 \n" // (x4 + x5) +  (x6 + x7)
        "adc r23,r25 \n"
        "sub r12,r24 \n" // (x4 + x5) - (x6 + x7)
        "sbc r13,r25 \n"

        // third set: c0
        "asr r7 \n" // divide by 2 to keep from overflowing
        "ror r6 \n"
        "movw r2,r6 \n" // make backup
        "asr r23 \n" // divide by 2 to keep from overflowing
        "ror r22 \n"
        "add r6,r22 \n" // [(x0 + x1) + (x2 + x3)] + [(x4 + x5) + (x6 + x7)]
        "adc r7,r23 \n"
        "sub r2,r22 \n" // [(x0 + x1) + (x2 + x3)] - [(x4 + x5) + (x6 + x7)]
        "sbc r3,r23 \n"

        // third set: c1 - multiply by .707
        // process (x4 - x5)*cos - use mulsu to keep scaling
        "ldi r20,0x82 \n" // load multiply register with 0.707
        "ldi r21,0x5a \n"
        "mulsu r17,r21 \n"
        "movw r24,r0 \n"
        "mul r16,r20 \n"
        "movw r22,r0 \n"
        "mulsu r17,r20 \n"
        "sbc r25,r15 \n"
        "add r23,r0 \n"
        "adc r24,r1 \n"
        "adc r25,r15 \n"
        "mul r21,r16 \n"
        "add r23,r0 \n"
        "adc r24,r1 \n"
        "adc r25,r15 \n"

        // add result to other parts
        "asr r9 \n" // divide by 2 to keep from overflowing
        "ror r8 \n"
        "movw r16,r8 \n" // make backup
        "add r8,r24 \n" // [(x0 - x1) + (x2 - x3)] + 2*(x4 - x5)*C1
        "adc r9,r25 \n"
        "sub r16,r24 \n" // [(x0 - x1) + (x2 - x3)] - 2*(x4 - x5)*c1
        "sbc r17,r25 \n"

        // third set: c2
        "asr r5 \n" // divide by 2 to keep from overflowing
        "ror r4 \n"
        "movw r26,r4 \n" // make backup
        "asr r13 \n" // divide by 2 to keep from overflowing
        "ror r12 \n"
        "add r4,r12 \n" // [(x0 + x1) - (x2 + x3)] + [(x4 + x5) - (x6 + x7)]
        "adc r5,r13 \n"
        "sub r26,r12 \n" // [(x0 + x1) - (x2 + x3)] - [(x4 + x5) - (x6 + x7)]
        "sbc r27,r13 \n"

        // third set: c3 - multiply by .707
        // process (x6 - x7)*cos - use mulsu to keep scaling
        "mulsu r19,r21 \n"
        "movw r24,r0 \n"
        "mul r18,r20 \n"
        "movw r22,r0 \n"
        "mulsu r19,r20 \n"
        "sbc r25,r15 \n"
        "add r23,r0 \n"
        "adc r24,r1 \n"
        "adc r25,r15 \n"
        "mul r21,r18 \n"
        "add r23,r0 \n"
        "adc r24,r1 \n"
        "adc r25,r15 \n"

        // add result to other parts
        "asr r11 \n" // divide by 2 to keep from overflowing
        "ror r10 \n"
        "movw r12,r10 \n" // make backup
        "add r10,r24 \n" // [(x0 - x1) - (x2 - x3)] + 2*(x6 - x7)*C1
        "adc r11,r25 \n"
        "sub r12,r24 \n" // [(x0 - x1) - (x2 - x3)] - 2*(x6 - x7)*c1
        "sbc r13,r25 \n"

        // restore all the data and repeat as necessary
        "st y+,r6 \n" // store x0
        "st y+,r7 \n"
        "st y+,r8 \n" // store x1
        "st y+,r9 \n"
        "st y+,r4 \n" // store x2
        "st y+,r5 \n"
        "st y+,r10 \n" // store x3
        "st y+,r11 \n"
        "st y+,r2 \n" // store x4
        "st y+,r3 \n"
        "st y+,r16 \n" // store x5
        "st y+,r17 \n"
        "st y+,r26 \n" // store x6
        "st y+,r27 \n"
        "st y+,r12 \n" // store x7
        "st y+,r13 \n"
        "dec r14 \n" // check if at end of data space
        "breq 4f \n"
        "rjmp 1b \n"
    );

    // remainder of the butterflies (fourth and higher)
    // initialize
    asm volatile (
        "4: \n"
        "ldi r16, 0x20 \n" // prep outer loop counter - full stride (x2 for 16b numbers)
        "mov r12,r16 \n"
        "clr r13 \n"
        "ldi r16, hi8((fht_input + " STRINGIFY(FHT_SAMPLES_N*2) ")) \n" // prep end of dataspace register
        "mov r9, r16 \n"
        "ldi r30, lo8(_cas_constants) \n" // initialize lookup table address
        "ldi r31, hi8(_cas_constants) \n"
        "ldi r16, 0x10 \n" // prep half stride length (x2 for 16b numbers)
        "mov r10,r16 \n"
        "clr r11 \n"
        "ldi r16,0x04 \n" // load inner loop counter to quarter stride (x1 for counter)
        "mov r14,r16 \n"

        // outer_loop - reset variables for next pass through the butterflies
        "5: \n"
        "ldi r26, lo8(fht_input) \n" //set top pointer to beginning of data space
        "ldi r27, hi8(fht_input) \n"
        "movw r28,r26 \n" // set bottom pointer to top
        "add r28,r10 \n" // add half stride to the bottom pointer
        "adc r29,r11 \n"
        "mov r8,r14 \n" // make backup of inner loop counter

        // inner_loop - do butterflies
        // first one is wk = (1,0)
        "6: \n"
        "ld r2,x+ \n" // fetch top half
        "ld r3,x \n"
        "ld r4,y \n" // fetch bottom half
        "ldd r5,y+1 \n"
        "asr r3 \n" // divide by 2 to keep from overflowing
        "ror r2 \n"
        "movw r6,r2 \n" // make backup
        "asr r5 \n" // divide by 2 to keep from overflowing
        "ror r4 \n"
        "add r6,r4 \n" // add for top half
        "adc r7,r5 \n"
        "sub r2,r4 \n" // subtract for bottom half
        "sbc r3,r5 \n"
        "st y+,r2 \n" // store bottom half
        "st y+,r3 \n"
        "st x,r7 \n" // store top half
        "st -x,r6 \n"
        "add r26,r12 \n"  // top pointer now becomes bottom pointer
        "adc r27,r13 \n"
        "sbiw r26,0x01 \n" // decrement to next butterfly
        "dec r14 \n" // weve done the first one

        // remainder are regular
        // muls replaced with mulsu to save a few cycles
        "7: \n"
        "ld r16,y \n" // fetch upper half
        "ldd r17,y+1 \n"
        "ld r19,x \n" // fetch lower half
        "ld r18,-x \n"
        "lpm r20,z+ \n" // fetch cosine
        "lpm r21,z+ \n"
        "lpm r22,z+ \n" // fetch sine
        "lpm r23,z+ \n"

        // process upper*cos
        "muls r17,r21 \n"
        "movw r4,r0 \n"
        "mul r16,r20 \n"
        "movw r2,r0 \n"
        "mulsu r17,r20 \n"
        "sbc r5,r15 \n"
        "add r3,r0 \n"
        "adc r4,r1 \n"
        "adc r5,r15 \n"
        "mul r21,r16 \n"
        "add r3,r0 \n"
        "adc r4,r1 \n"
        "adc r5,r15 \n"

        // process lower*sin and accumulate
        "muls r19,r23 \n"
        "movw r6,r0 \n"
        "mul r18,r22 \n"
        "add r2,r0 \n"
        "adc r3,r1 \n"
        "adc r4,r6 \n"
        "adc r5,r7 \n"
        "mulsu r19,r22 \n"
        "sbc r5,r15 \n"
        "add r3,r0 \n"
        "adc r4,r1 \n"
        "adc r5,r15 \n"
        "mul r23,r18 \n"
        "add r3,r0 \n"
        "adc r4,r1 \n"
        "adc r5,r15 \n"

        // do upper butterfly - faster than doing a seperate loop
        "movw r0,r28 \n" // backup pointer
        "sub r28,r10 \n" // index to top half - subtract half stride
        "sbc r29,r11 \n"
        "ld r2,y \n" // fetch top half
        "ldd r3,y+1 \n"
        "asr r3 \n" // divide by 2 to keep from overflowing
        "ror r2 \n"
        "movw r6,r2 \n" // make backup
        "add r6,r4 \n" // add for top half
        "adc r7,r5 \n"
        "sub r2,r4 \n" // subtract for bottom half
        "sbc r3,r5 \n"
        "st y,r6 \n" // store top half
        "std y+1,r7 \n"
        "movw r28,r0 \n" // restore pointer
        "st y+,r2 \n" // store bottom half
        "st y+,r3 \n"

        // process upper*sin
        "muls r17,r23 \n"
        "movw r4,r0 \n"
        "mul r16,r22 \n"
        "movw r2,r0 \n"
        "mulsu r17,r22 \n"
        "sbc r5,r15 \n"
        "add r3,r0 \n"
        "adc r4,r1 \n"
        "adc r5,r15 \n"
        "mul r23,r16 \n"
        "add r3,r0 \n"
        "adc r4,r1 \n"
        "adc r5,r15 \n"

        // process lower*cos and accumulate (subtract)
        "muls r19,r21 \n"
        "movw r6,r0 \n"
        "mul r18,r20 \n"
        "sub r2,r0 \n"
        "sbc r3,r1 \n"
        "sbc r4,r6 \n"
        "sbc r5,r7 \n"
        "mulsu r19,r20 \n"
        "adc r5,r15 \n"
        "sub r3,r0 \n"
        "sbc r4,r1 \n"
        "sbc r5,r15 \n"
        "mul r21,r18 \n"
        "sub r3,r0 \n"
        "sbc r4,r1 \n"
        "sbc r5,r15 \n"

        // do lower butterfly - faster than doing a seperate loop
        "movw r0,r26 \n" // backup pointer
        "sub r26,r10 \n" // index to top half - subtract half stride
        "sbc r27,r11 \n"
        "ld r2,x+ \n" // fetch top half
        "ld r3,x \n"
        "asr r3 \n" // divide by 2 to keep from overflowing
        "ror r2 \n"
        "movw r6,r2 \n" // make backup
        "add r6,r4 \n" // add for top half
        "adc r7,r5 \n"
        "sub r2,r4 \n" // subtract for bottom half
        "sbc r3,r5 \n"
        "st x,r7 \n" // store top half
        "st -x,r6 \n"
        "movw r26,r0 \n" // restore pointer
        "st x+,r2 \n" // store bottom half
        "st x,r3 \n"
        "sbiw r26,0x02 \n" // decrement to next butterfly

        // check where we are in the process
        "dec r14 \n" // check which butterfly were doing
        "breq 8f \n" // finish off if last one
        "rjmp 7b \n" // go back and do it again if not done yet

        // last buttefly is wk = (0,1)
        "8: \n"
        "movw r26,r28 \n" // backup pointer
        "sub r26,r10 \n" // index to top half - subtract half stride
        "sbc r27,r11 \n"
        "ld r2,x+ \n" // fetch top half
        "ld r3,x \n"
        "ld r4,y \n" // fetch bottom half
        "ldd r5,y+1 \n"
        "asr r3 \n" // divide by 2 to keep from overflowing
        "ror r2 \n"
        "movw r6,r2 \n" // make backup
        "asr r5 \n" // divide by 2 to keep from overflowing
        "ror r4 \n"
        "add r6,r4 \n" // add for top half
        "adc r7,r5 \n"
        "sub r2,r4 \n" // subtract for bottom half
        "sbc r3,r5 \n"
        "st y,r2 \n" // store bottom half
        "std y+1,r3 \n"
        "st x,r7 \n" // store top half
        "st -x,r6 \n"

        // reset for next pass
        "movw r24,r10 \n" // make a quarter stride
        "lsr r25 \n"
        "ror r24 \n"
        "add r28,r24 \n" // lower is incremented by quarter stride to get to the next butterfly
        "adc r29,r25 \n"
        "cpi r28, lo8(fht_input + " STRINGIFY(FHT_SAMPLES_N*2) ") \n" // check if at end of dataspace
        "cpc r29, r9 \n"
        "brsh 10f \n"
        "movw r26,r28 \n" // bottom is now top
        "add r28,r10 \n" // bottom is moved down half a stride
        "adc r29,r11 \n"
        "mov r14,r8 \n" // reset inner loop counter
        "sub r30,r10 \n" // reset Wk lookup table pointer
        "sbc r31,r11 \n"
        "adiw r30,0x04 \n" // number of butterflies minus 1 for the ones not done
        "rjmp 6b \n" // keep going

        // inner_done - reset for next set of butteflies
        "10: \n"
        "sbrc r8, " STRINGIFY(LOG_N - 2) " \n" // check if finished with all butteflies
        "rjmp 11f \n"
        "mov r14,r10 \n" // set inner loop count to half stride - works for N <= 256
        "lsr r14 \n" // divide inner loop counter by 2
        "movw r10,r12 \n" // set new half stride to old full stride
        "lsl r12 \n" // multiply full stride by 2
        "rol r13 \n"
        "rjmp 5b \n" // keep going
        "11: \n" // rest of code here
        : :
        : "r0", "r18", "r19", "r20", "r21", "r22", "r23", "r24", "r25", "r26", "r27", "r30", "r31" // clobber list for whole thing
    );

    // get the clobbers off the stack
    asm volatile (
        "pop r29 \n"
        "pop r28 \n"
        "pop r17 \n"
        "pop r16 \n"
        "pop r15 \n"
        "pop r14 \n"
        "pop r13 \n"
        "pop r12 \n"
        "pop r11 \n"
        "pop r10 \n"
        "pop r9 \n"
        "pop r8 \n"
        "pop r7 \n"
        "pop r6 \n"
        "pop r5 \n"
        "pop r4 \n"
        "pop r3 \n"
        "pop r2 \n"
        "clr r1 \n" // reset the c compiler null register
    );
}

#endif //AVR_FHT_FHT_TRANSFORM_H