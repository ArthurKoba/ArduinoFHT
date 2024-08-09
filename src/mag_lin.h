#ifndef AVR_FHT_MAG_LIN_H
#define AVR_FHT_MAG_LIN_H

#ifndef LIN_OUT // wether using the linear output function or not
#define LIN_OUT 0
#endif

#if (LIN_OUT == 1)
namespace FHT {

    uint16_t __attribute__((used)) fht_lin_out[(FHT_N/2)]; // FHT linear output magintude buffer

    extern const uint8_t __attribute__((used)) _lin_table[] PROGMEM = {
        #include <sqrtlookup16.inc>
    };
}
#endif

static inline void fht_mag_lin(void) {
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
        "push r15 \n"
        "push r16 \n"
        "push r17 \n"
        "push r28 \n"
        "push r29 \n"
    );

    // this returns an 16b unsigned value which is 16*((img^2 + real^2)^0.5)
    asm volatile (
        "ldi r26, lo8(fht_input) \n" // set to beginning of data space
        "ldi r27, hi8(fht_input) \n"
        "ldi r28, lo8(fht_input) \n" // set to beginning of result space
        "ldi r29, hi8(fht_input) \n"
        "ldi r30, lo8(fht_input + " STRINGIFY(FHT_N*2) ") \n" // set to end of data space
        "ldi r31, hi8(fht_input + " STRINGIFY(FHT_N*2) ") \n"
        "movw r8,r30 \n" // z register clobbered below
        "clr r15 \n" // clear null register
        "ldi r20, " STRINGIFY(FHT_N/2) " \n" // set loop counter
        "ld r16,x+ \n" // do zero frequency bin first
        "ld r17,x+ \n"
        "movw r18,r16 \n" // double zero frequency bin
        "rjmp 20f \n" // skip ahead

        "1: \n"
        "movw r30,r8 \n" // restore z register
        "ld r16,x+ \n" // fetch real
        "ld r17,x+ \n"
        "ld r19,-Z \n" // fetch imaginary
        "ld r18,-Z \n"
        "movw r8,r30 \n" // store z register

        // process real^2
        "20: \n"
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

        // square root via lookup table
        // first scales the magnitude to a 16b value times an 8b exponent
        "clr r17 \n" // clear exponent register
        "tst r5 \n"
        "breq 3f \n"
        "ldi r17,0x08 \n"
        "movw r30,r4 \n"

        "2: \n"
        "cpi r31,0x40 \n"
        "brsh 6f \n" // all values already known to be > 0x40
        "lsl r3 \n"
        "rol r30 \n"
        "rol r31 \n"
        "lsl r3 \n"
        "rol r30 \n"
        "rol r31 \n"
        "dec r17 \n"
        "rjmp 6f \n"

        "3: \n"
        "tst r4 \n"
        "breq 5f \n"
        "ldi r17,0x04 \n"
        "mov r31,r4 \n"
        "mov r30,r3 \n"

        "4: \n"
        "cpi r31,0x40 \n"
        "brsh 6f \n" // all values already known to be > 0x40
        "lsl r2 \n"
        "rol r30 \n"
        "rol r31 \n"
        "lsl r2 \n"
        "rol r30 \n"
        "rol r31 \n"
        "dec r17 \n"
        "rjmp 4b \n"

        // find sqrt via lookup table
        "5: \n"
        "movw r30,r2 \n"
        "cpi r31,0x40 \n"
        "brsh 6f \n"
        "cpi r31,0x10 \n"
        "brsh 12f \n"
        "cpi r31,0x01 \n"
        "brlo 10f \n"
        "swap r31 \n"
        "swap r30 \n"
        "andi r30,0x0f \n"
        "or r30,r31 \n"
        "lsr r30 \n"
        "ldi r31,0x01 \n"
        "rjmp 10f \n"

        "6: \n"
        "mov r30,r31 \n"
        "ldi r31,0x02 \n"
        "rjmp 10f \n"

        "12: \n"
        "lsl r30 \n"
        "rol r31 \n"
        "mov r30,r31 \n"
        "ori r30,0x80 \n"
        "ldi r31,0x01 \n"

        "10: \n"
        "subi r30, lo8(-(_lin_table)) \n" // add offset to lookup table pointer
        "sbci r31, hi8(-(_lin_table)) \n"
        "lpm r16,z \n" // fetch square root
        "clr r18 \n"

        "7: \n" // multiply by exponent
        "tst r17 \n"
        "breq 8f \n" // skip if no exponent
        "13: \n"
        "lsl r16 \n"
        "rol r18 \n"
        "dec r17 \n"
        "brne 13b \n"

        "8: \n"
        "st y+,r16 \n" // store value
        "st y+,r18 \n"
        "dec r20 \n" // check if all data processed
        "breq 9f \n"
        "rjmp 1b \n"
        "9: \n" // all done
        : :
        : "r0", "r26", "r27", "r30", "r31", "r18", "r19", "r20"// clobber list
    );

    // get the clobbers off the stack
    asm volatile (
        "pop r29 \n"
        "pop r28 \n"
        "pop r17 \n"
        "pop r16 \n"
        "pop r15 \n"
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

#endif //AVR_FHT_MAG_LIN_H