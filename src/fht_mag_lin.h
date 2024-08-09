#ifndef AVR_FHT_FHT_MAG_LIN_H
#define AVR_FHT_FHT_MAG_LIN_H

#include <AvrFHT.h>

uint16_t __attribute__((used)) fht_lin_out[FHT_AMPLITUDES_N]; // FHT linear output magintude buffer

/*
  sqrtlookup16.inc - square root lookup table - 768 bytes
  broken into 4 sections, with padding between sections
  table0 : $000 -> $0ff : sqrt($0000 -> $00ff)
  table1 : $100 -> $17f : sqrt($0100 -> $0fff)
  table2 : $180 -> $1ff : sqrt($1000 -> $3fff)
  table3 : $200 -> $2ff   sqrt($4000 -> $ffff)
  you can easily check which bin your data falls into
  by checking which high bits are set.  it is best to
  check from high bits to low bits, as the lower bins
  require less modification for the data fetch.

  could be optimized by moving the tables around -
  table1 takes a lot of modifications for the fetch.

  sample code: assumes data is in z register - havent tested it yet

  cpi zh,$40
  brsh table3
  cpi zh,$10
  brsh table2
  cpi zh,$01
  brlo done
  swap zh
  swap zl
  andi zl,$0f
  or zl,zh
  lsr zl
  ldi zh,$01
  rjmp done

  table3:

  mov zl,zh
  ldi zh,$02
  rjmp done

  table2:

  lsl zl
  rol zh
  mov zl,zh
  ori zl,$80
  ldi zh,$01

  done:

  lpm r16,z
*/

extern const uint8_t __attribute__((used)) _lin_table[] PROGMEM = {
    0,1,1,2,2,2,2,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,0,6,8,10,11,13,14,15,16,17,18,19,20,20,21,22,23,23,24,25,25,26,27,27,28,28,29,29,30,30,31,31,32,32,33,33,34,34,35,35,36,36,37,37,38,38,38,39,39,40,40,40,41,41,42,42,42,43,43,43,44,44,45,45,45,46,46,46,47,47,47,48,48,48,49,49,49,50,50,50,51,51,51,52,52,52,52,53,53,53,54,54,54,55,55,55,55,56,56,56,57,57,57,57,58,58,58,59,59,59,59,60,60,60,60,61,61,61,61,62,62,62,62,63,63,63,63,64,0,11,16,20,23,25,28,30,32,34,36,38,39,41,42,44,45,47,48,49,51,52,53,54,55,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,72,73,74,75,76,77,78,78,79,80,81,82,82,83,84,85,85,86,87,88,88,89,90,91,91,92,93,93,94,95,95,96,97,97,98,99,99,100,101,101,102,102,103,104,104,105,106,106,107,107,108,109,109,110,110,111,111,112,113,113,114,114,115,115,116,116,117,118,118,119,119,120,120,121,121,122,122,123,123,124,124,125,125,126,126,127,127,0,16,23,28,32,36,39,42,45,48,51,53,55,58,60,62,64,66,68,70,72,73,75,77,78,80,82,83,85,86,88,89,91,92,93,95,96,97,99,100,101,102,104,105,106,107,109,110,111,112,113,114,115,116,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,139,140,141,142,143,144,145,146,147,148,148,149,150,151,152,153,153,154,155,156,157,158,158,159,160,161,162,162,163,164,165,166,166,167,168,169,169,170,171,172,172,173,174,175,175,176,177,177,178,179,180,180,181,182,182,183,184,185,185,186,187,187,188,189,189,190,191,191,192,193,193,194,195,195,196,197,197,198,199,199,200,200,201,202,202,203,204,204,205,206,206,207,207,208,209,209,210,210,211,212,212,213,213,214,215,215,216,216,217,218,218,219,219,220,221,221,222,222,223,223,224,225,225,226,226,227,227,228,229,229,230,230,231,231,232,232,233,234,234,235,235,236,236,237,237,238,238,239,239,240,241,241,242,242,243,243,244,244,245,245,246,246,247,247,248,248,249,249,250,250,251,251,252,252,253,253,254,254,255,255
};

static inline void fht_mag_lin() {
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

#endif //AVR_FHT_FHT_MAG_LIN_H