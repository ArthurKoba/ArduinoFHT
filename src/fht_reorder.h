#ifndef AVR_FHT_FHT_REORDER_H
#define AVR_FHT_FHT_REORDER_H

#include <AvrFHT.h>

// fft butterfly swap pairs
// reorders input values for butterfly operations
extern const uint8_t __attribute__((used)) _reorder_table[] PROGMEM = {
#if (FHT_SAMPLES_N == 256)
    240,15,248,31,244,47,252,63,242,79,250,95,246,111,254,127,241,143,249,159,245,175,253,191,243,207,251,223,247,239,224,7,232,23,228,39,236,55,226,71,234,87,230,103,238,119,225,135,233,151,229,167,237,183,227,199,235,215,208,11,216,27,212,43,220,59,210,75,218,91,214,107,222,123,209,139,217,155,213,171,221,187,211,203,192,3,200,19,196,35,204,51,194,67,202,83,198,99,206,115,193,131,201,147,197,163,205,179,176,13,184,29,180,45,188,61,178,77,186,93,182,109,190,125,177,141,185,157,181,173,160,5,168,21,164,37,172,53,162,69,170,85,166,101,174,117,161,133,169,149,144,9,152,25,148,41,156,57,146,73,154,89,150,105,158,121,145,137,128,1,136,17,132,33,140,49,130,65,138,81,134,97,142,113,112,14,120,30,116,46,124,62,114,78,122,94,118,110,96,6,104,22,100,38,108,54,98,70,106,86,80,10,88,26,84,42,92,58,82,74,64,2,72,18,68,34,76,50,48,12,56,28,52,44,32,4,40,20,16,8
#elif (FHT_SAMPLES_N == 128)
    112,7,120,15,116,23,124,31,114,39,122,47,118,55,126,63,113,71,121,79,117,87,125,95,115,103,123,111,96,3,104,11,100,19,108,27,98,35,106,43,102,51,110,59,97,67,105,75,101,83,109,91,80,5,88,13,84,21,92,29,82,37,90,45,86,53,94,61,81,69,89,77,64,1,72,9,68,17,76,25,66,33,74,41,70,49,78,57,48,6,56,14,52,22,60,30,50,38,58,46,32,2,40,10,36,18,44,26,16,4,24,12
#elif (FHT_SAMPLES_N == 64)
    56,7,60,15,58,23,62,31,57,39,61,47,59,55,48,3,52,11,50,19,54,27,49,35,53,43,40,5,44,13,42,21,46,29,41,37,32,1,36,9,34,17,38,25,24,6,28,14,26,22,16,2,20,10,8,4
#elif (FHT_SAMPLES_N == 32)
    24,3,28,7,26,11,30,15,25,19,29,23,16,1,20,5,18,9,22,13,8,2,12,6
#elif (FHT_SAMPLES_N == 16)
    12,3,14,7,13,11,8,1,10,5,4,2
#endif
};

static inline void fht_reorder() {
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
        "ldi r20, " STRINGIFY((FHT_SAMPLES_N / 2) - _R_V) " \n" // set to first sample

        // get source sample
        "1: \n"
        "lpm r26,z+ \n" // fetch source address
        "clr r27 \n" // these next 3 lines could be optimized out
        "lsl r26 \n" // by chaging the lookup table
        "rol r27 \n" // only works for FHT_SAMPLES_N <= 128
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

#endif //AVR_FHT_FHT_REORDER_H