#ifndef AVR_FHT_REORDER_H
#define AVR_FHT_REORDER_H

#ifndef REORDER // wether using the reorder function or not
#define REORDER 1
#endif

#if (REORDER == 1)
namespace FHT {

    extern const uint8_t __attribute__((used)) _reorder_table[] PROGMEM = {
#if (FHT_N == 256)
        #include <256_reorder.inc>
#elif (FHT_N == 128)
        #include <128_reorder.inc>
#elif (FHT_N == 64)
        #include <64_reorder.inc>
#elif (FHT_N == 32)
        #include <32_reorder.inc>
#elif (FHT_N == 16)
        #include <16_reorder.inc>
#endif
    };
}
#endif

static inline void fht_reorder(void) {
    // save registers that are getting clobbered
    // avr-gcc requires r2:r17,r28:r29, and r1 cleared
    asm volatile (
        "push r2 \n"
        "push r3 \n"
        "push r6 \n"
        "push r7 \n"
        "push r28 \n"
        "push r29 \n"
    );

    // move values to bit reversed locations
    asm volatile (
        "ldi r30, lo8(_reorder_table) \n" // initialize lookup table address
        "ldi r31, hi8(_reorder_table) \n"
        "ldi r20, " STRINGIFY((FHT_N/2) - _R_V) " \n" // set to first sample

        // get source sample
        "1: \n"
        "lpm r26,z+ \n" // fetch source address
        "clr r27 \n" // these next 3 lines could be optimized out
        "lsl r26 \n" // by chaging the lookup table
        "rol r27 \n" // only works for FHT_N <= 128
        "subi r26, lo8(-(fht_input)) \n" // pointer to offset
        "sbci r27, hi8(-(fht_input)) \n"
        "ld r2,x+ \n" // fetch source
        "ld r3,x \n"

        // find destination
        "lpm r28,z+ \n"
        "clr r29 \n" // same here
        "lsl r28 \n" // multiply offset by 2
        "rol r29 \n"
        "subi r28, lo8(-(fht_input)) \n" // add pointer to offset
        "sbci r29, hi8(-(fht_input)) \n"
        "ld r6,y \n" // fetch destination
        "ldd r7,y+1 \n"

        // swap source and destination samples
        "st x,r7 \n"
        "st -x,r6 \n"
        "st y,r2 \n"
        "std y+1,r3 \n"

        // check if done
        "dec r20 \n" // go to next sample
        "brne 1b \n" // finish off if last sample
        : :
        : "r20", "r26", "r27", "r30", "r31" // clobber list
    );

    // get the clobbers off the stack
    asm volatile (
        "pop r29 \n"
        "pop r28 \n"
        "pop r7 \n"
        "pop r6 \n"
        "pop r3 \n"
        "pop r2 \n"
    );
}

#endif //AVR_FHT_REORDER_H