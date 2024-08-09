#ifndef AVR_FHT_FHT_WINDOW_H
#define AVR_FHT_FHT_WINDOW_H

#include <FHT.h>

extern const int16_t __attribute__((used)) _window_func[] PROGMEM = {
#if (FHT_SAMPLES_N ==  256)
    #include <hann_256.inc>
#elif (FHT_SAMPLES_N ==  128)
    #include <hann_128.inc>
#elif (FHT_SAMPLES_N ==  64)
    #include <hann_64.inc>
#elif (FHT_SAMPLES_N ==  32)
    #include <hann_32.inc>
#elif (FHT_SAMPLES_N ==  16)
    #include <hann_16.inc>
#endif
};

static inline void fht_window() {
    // save registers that are getting clobbered
    // avr-gcc requires r2:r17,r28:r29, and r1 cleared
    asm volatile (
        "push r2 \n"
        "push r3 \n"
        "push r4 \n"
        "push r5 \n"
        "push r15 \n"
        "push r16 \n"
        "push r17 \n"
        "push r28 \n"
        "push r29 \n"
    );

    // this applies a window to the data for better frequency resolution
    asm volatile (
        "ldi r28, lo8(fht_input) \n" // set to beginning of data space
        "ldi r29, hi8(fht_input) \n"
        "ldi r30, lo8(_window_func) \n" // set to beginning of lookup table
        "ldi r31, hi8(_window_func) \n"
        "clr r15 \n" // prep null register
        "ldi r20, " STRINGIFY(((FHT_SAMPLES_N) & (0xff))) " \n"

        "1: \n"
        "lpm r22,z+ \n" // fetch window value
        "lpm r23,z+ \n"
        "ld r16,y \n" // fetch data
        "ldd r17,y+1 \n"

        // multiply by window
        "fmuls r17,r23 \n"
        "movw r4,r0 \n"
        "fmul r16,r22 \n"
        "adc r4,r15 \n"
        "movw r2,r0 \n"
        "fmulsu r17,r22 \n"
        "sbc r5,r15 \n"
        "add r3,r0 \n"
        "adc r4,r1 \n"
        "adc r5,r15 \n"
        "fmulsu r23,r16 \n"
        "sbc r5,r15 \n"
        "add r3,r0 \n"
        "adc r4,r1 \n"
        "adc r5,r15 \n"

        "st y+,r4 \n" // restore data
        "st y+,r5 \n"
        "dec r20 \n" // check if done
        "brne 1b \n"
        : :
        : "r0", "r20", "r30", "r31", "r22", "r23" // clobber list
    );

    // get the clobbers off the stack
    asm volatile (
        "pop r29 \n"
        "pop r28 \n"
        "pop r17 \n"
        "pop r16 \n"
        "pop r15 \n"
        "pop r5 \n"
        "pop r4 \n"
        "pop r3 \n"
        "pop r2 \n"
        "clr r1 \n" // reset c compiler null register
    );
}

#endif //AVR_FHT_FHT_WINDOW_H