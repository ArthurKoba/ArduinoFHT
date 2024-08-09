#ifndef AVR_FHT_FHT_MAG_LOG_H
#define AVR_FHT_FHT_MAG_LOG_H

#include <FHT.h>

uint8_t __attribute__((used)) fht_log_out[(FHT_N/2)]; // FHT log output magintude buffer

extern const uint8_t __attribute__((used)) _log_table[] PROGMEM = {
    #include <decibel.inc>
};

static inline void fht_mag_log() {
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
        "push r15 \n"
        "push r16 \n"
        "push r17 \n"
        "push r28 \n"
        "push r29 \n"
    );

    // this returns an 8b unsigned value which is 16*log2((img^2 + real^2)^0.5)
    asm volatile (
        "ldi r26, lo8(fht_input) \n" // set to beginning of data space
        "ldi r27, hi8(fht_input) \n"
        "ldi r28, lo8(fht_log_out) \n" // set to beginning of result space
        "ldi r29, hi8(fht_log_out) \n"
        "ldi r30, lo8(fht_input + " STRINGIFY(FHT_N*2) ") \n" // set to end of data space
        "ldi r31, hi8(fht_input + " STRINGIFY(FHT_N*2) ") \n"
        "movw r8,r30 \n" // z register clobbered below
        "clr r15 \n" // clear null register
        "ldi r20, " STRINGIFY(FHT_N/2) " \n" // set loop counter
        "ld r16,x+ \n" // do zero frequency bin first
        "ld r17,x+ \n"
        "movw r18,r16 \n" // double zero frequency bin
        "rjmp 10f \n" // skip ahead

        "1: \n"
        "movw r30,r8 \n" // restore z register
        "ld r16,x+ \n" // fetch real
        "ld r17,x+ \n"
        "ld r19,-Z \n" // fetch imaginary
        "ld r18,-Z \n"
        "movw r8,r30 \n" // store z register

        // process real^2
        "10: \n"
        "muls r17,r17 \n"
        "movw r4,r0 \n"
        "mul r16,r16 \n"
        "movw r2,r0 \n"
        "fmulsu r17,r16 \n" // automatically does x2
        "sbc r5,r15 \n"
        "add r3,r0 \n"
        "adc r4,r1 \n"
        "adc r5,r15 \n"

        // process img^2 and accumulate
        "muls r19,r19 \n"
        "movw r6,r0 \n"
        "mul r18,r18 \n"
        "add r2,r0 \n"
        "adc r3,r1 \n"
        "adc r4,r6 \n"
        "adc r5,r7 \n"
        "fmulsu r19,r18 \n" // automatically does x2
        "sbc r5,r15 \n"
        "add r3,r0 \n"
        "adc r4,r1 \n"
        "adc r5,r15 \n"

        // decibel of the square root via lookup table
        // scales the magnitude to an 8b value times an 8b exponent
        "clr r17 \n" // clear exponent register
        "tst r5 \n"
        "breq 3f \n"
        "ldi r17,0x0c \n"
        "mov r30,r5 \n"

        "2: \n"
        "cpi r30,0x40 \n"
        "brsh 8f \n"
        "lsl r4 \n"
        "rol r30 \n"
        "lsl r4 \n"
        "rol r30 \n"
        "dec r17 \n"
        "rjmp 2b \n"

        "3: \n"
        "tst r4 \n"
        "breq 5f \n"
        "ldi r17,0x08 \n"
        "mov r30,r4 \n"

        "4: \n"
        "cpi r30,0x40 \n"
        "brsh 8f \n"
        "lsl r3 \n"
        "rol r30 \n"
        "lsl r3 \n"
        "rol r30 \n"
        "dec r17 \n"
        "rjmp 4b \n"

        "5: \n"
        "tst r3 \n"
        "breq 7f \n"
        "ldi r17,0x04 \n"
        "mov r30,r3 \n"

        "6: \n"
        "cpi r30,0x40 \n"
        "brsh 8f \n"
        "lsl r2 \n"
        "rol r30 \n"
        "lsl r2 \n"
        "rol r30 \n"
        "dec r17 \n"
        "rjmp 6b \n"

        "7: \n"
        "mov r30,r2 \n"

        "8: \n"
        "clr r31 \n"
        "subi r30, lo8(-(_log_table)) \n" // add offset to lookup table pointer
        "sbci r31, hi8(-(_log_table)) \n"
        "lpm r16,z \n" // fetch log compressed square root
        "swap r17 \n"  // multiply exponent by 16
        "add r16,r17 \n" // add for final value
        "st y+,r16 \n" // store value
        "dec r20 \n" // check if all data processed
        "breq 9f \n"
        "rjmp 1b \n"
        "9: \n" // all done
        : :
        : "r0", "r26", "r27", "r30", "r31", "r18", "r19", "r20" // clobber list
    );

    // get the clobbers off the stack
    asm volatile (
        "pop r29 \n"
        "pop r28 \n"
        "pop r17 \n"
        "pop r16 \n"
        "pop r15 \n"
        "pop r9 \n"
        "pop r8 \n"
        "pop r7 \n"
        "pop r6 \n"
        "pop r5 \n"
        "pop r4 \n"
        "pop r3 \n"
        "pop r2 \n"
        "clr r1 \n" // reset c compiler null register
    );
}

#endif //AVR_FHT_FHT_MAG_LOG_H