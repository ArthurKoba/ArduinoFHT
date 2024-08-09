#ifndef AVR_FHT_MAG_LIN8_H
#define AVR_FHT_MAG_LIN8_H

#ifndef SCALE // scaling factor for lin8 output function
#define SCALE 1
#endif

#ifndef LIN_OUT8 // wether using the linear output function or not
#define LIN_OUT8 0
#endif

#if (LIN_OUT8 == 1)
namespace FHT {

    uint8_t __attribute__((used)) fht_lin_out8[(FHT_N/2)]; // FHT linear output magintude buffer

    extern const uint8_t __attribute__((used)) _lin_table8[] PROGMEM = {
        #include <sqrtlookup8.inc>
    };
}
#endif

static inline void fht_mag_lin8(void) {
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

    // this returns an 8b unsigned value which is (225/(181*256*256))*((img^2 + real^2)^0.5)
    asm volatile (
        "ldi r26, lo8(fht_input) \n" // set to beginning of data space
        "ldi r27, hi8(fht_input) \n"
        "ldi r28, lo8(fht_lin_out8) \n" // set to beginning of result space
        "ldi r29, hi8(fht_lin_out8) \n"
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

#if (SCALE == 1)
        "movw r30,r4 \n"
#elif (SCALE == 2)
        "lsl r3 \n"
        "rol r4 \n"
        "rol r5 \n"
        "movw r30,r4 \n"
#elif (SCALE == 4)
        "lsl r3 \n"
        "rol r4 \n"
        "rol r5 \n"
        "lsl r3 \n"
        "rol r4 \n"
        "rol r5 \n"
        "movw r30,r4 \n"
#elif (SCALE == 128)
        "lsr r5 \n"
        "ror r4 \n"
        "ror r3 \n"
        "mov r31,r4 \n"
        "mov r30,r3 \n"
#elif (SCALE == 256)
        "mov r30,r3 \n"
        "mov r31,r4 \n"
#else
        "ldi r18, " STRINGIFY(SCALE) " \n"
        "mul r5,r18 \n"
        "mov r31,r0 \n"
        "mul r3,r18 \n"
        "mov r30,r1 \n"
        "mul r4,r18 \n"
        "add r30,r0 \n"
        "adc r31,r1 \n"
#endif

        // square root via lookup table
        // scales the magnitude to an 8b value
        "cpi r31,0x10 \n"
        "brsh 2f \n"
        "cpi r31,0x01 \n"
        "brsh 3f \n"
        "rjmp 6f \n"

        "2: \n"
        "lsl r30 \n"
        "rol r31 \n"
        "mov r30,r31 \n"
        "ldi r31,0x01 \n"
        "subi r30,0x80 \n"
        "sbci r31,0xff \n"
        "rjmp 6f \n"

        "3: \n"
        "swap r30 \n"
        "swap r31 \n"
        "andi r30,0x0f \n"
        "or r30,r31 \n"
        "lsr r30 \n"
        "ldi r31,0x01 \n"

        "6: \n"
        "subi r30, lo8(-(_lin_table8)) \n" // add offset to lookup table pointer
        "sbci r31, hi8(-(_lin_table8)) \n"
        "lpm r16,z \n" // fetch log compressed square root
        "st y+,r16 \n" // store value
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

#endif //AVR_FHT_MAG_LIN8_H