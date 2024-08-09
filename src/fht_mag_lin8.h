#ifndef AVR_FHT_FHT_MAG_LIN8_H
#define AVR_FHT_FHT_MAG_LIN8_H

#include <AvrFHT.h>

#ifndef SCALE_LIN8 // scaling factor for lin8 output function
#define SCALE_LIN8 1
#endif

uint8_t __attribute__((used)) fht_lin_out8[FHT_AMPLITUDES_N]; // FHT linear output magintude buffer

/*
 sqrt lookup table for 16b unsigned input values
 returns an 8b unsigned value
 this table is optimized for the fft program
 as the max value the sum of 2 squares can return is $8000
 so the top half is truncated off
 and then it is stretched to return 0 - 255
 the table is allocated as follows
 table0 : $000 -> $0ff : sqrt($0000 -> $00ff)
 table2 : $100 -> $17f : sqrt($0100 -> $0fff)
 table1 : $180 -> $27f : sqrt($1000 -> $7fff)

 its hacked to compensate for the $8000 problem

 sample code for doing the lookup table - not tested
 assumes data in z register

 cpi zh,$10
 brsh table2
 cpi zh,$01
 brsh table1
 rjmp done

 table2:

 lsl zl
 rol zh
 mov zl,zh
 ldi zh,$01
 rjmp done

 table1:

 swap zh
 swap zl
 andi zl,$0f
 or zl,zh
 lsr zl
 ldi zh,$02

 done:

 lpm r16,z

*/

extern const uint8_t __attribute__((used)) _lin_table8[] PROGMEM = {
    0,1,2,2,3,3,3,4,4,4,4,5,5,5,5,5,6,6,6,6,6,6,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,9,9,10,10,10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,11,11,11,11,12,12,12,12,12,12,12,12,12,12,12,12,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,14,14,14,14,14,14,14,14,14,14,14,14,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,0,8,11,14,16,18,20,21,23,24,25,26,28,29,30,31,32,33,34,35,36,37,37,38,39,40,41,41,42,43,44,44,45,46,46,47,48,48,49,50,50,51,52,52,53,53,54,55,55,56,56,57,57,58,59,59,60,60,61,61,62,62,63,63,64,64,65,65,66,66,67,67,68,68,69,69,69,70,70,71,71,72,72,73,73,73,74,74,75,75,76,76,76,77,77,78,78,78,79,79,80,80,80,81,81,82,82,82,83,83,84,84,84,85,85,85,86,86,87,87,87,88,88,88,89,89,89,90,255,16,23,28,32,36,39,42,45,48,50,53,55,57,60,62,64,66,68,69,71,73,75,76,78,80,81,83,84,86,87,89,90,92,93,94,96,97,98,100,101,102,103,105,106,107,108,109,110,112,113,114,115,116,117,118,119,120,121,122,123,124,126,127,128,129,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,143,144,145,146,147,148,149,150,150,151,152,153,154,155,155,156,157,158,159,159,160,161,162,163,163,164,165,166,166,167,168,169,169,170,171,172,172,173,174,175,175,176,177,177,178,179,180,180,181,182,182,183,184,185,185,186,187,187,188,189,189,190,191,191,192,193,193,194,195,195,196,197,197,198,198,199,200,200,201,202,202,203,203,204,205,205,206,207,207,208,208,209,210,210,211,211,212,213,213,214,214,215,216,216,217,217,218,219,219,220,220,221,221,222,223,223,224,224,225,225,226,227,227,228,228,229,229,230,230,231,232,232,233,233,234,234,235,235,236,236,237,237,238,239,239,240,240,241,241,242,242,243,243,244,244,245,245,246,246,247,247,248,248,249,249,250,251,251,252,252,253,253,254,254,255
};

static inline void fht_mag_lin8() {
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
        "ldi r30, lo8(fht_input + " STRINGIFY(FHT_SAMPLES_N * 2) ") \n" // set to end of data space
        "ldi r31, hi8(fht_input + " STRINGIFY(FHT_SAMPLES_N * 2) ") \n"
        "movw r8,r30 \n" // z register clobbered below
        "clr r15 \n" // clear null register
        "ldi r20, " STRINGIFY(FHT_SAMPLES_N / 2) " \n" // set loop counter
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

        #if (SCALE_LIN8 == 1)
        "movw r30,r4 \n"
        #elif (SCALE_LIN8 == 2)
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

#endif //AVR_FHT_FHT_MAG_LIN8_H