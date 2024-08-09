#ifndef AVR_FHT_FHT_MAG_LIN8_NO_ASM_H
#define AVR_FHT_FHT_MAG_LIN8_NO_ASM_H

#include <AvrFHT.h>
#include "fast_math.h"

uint8_t __attribute__((used)) fht_lin_out8[FHT_AMPLITUDES_N]; // FHT linear output magintude buffer


void fht_mag_lin8_no_asm() {
    for (int i = 0; i < FHT_AMPLITUDES_N; ++i) {
        uint16_t rss_value = fast_rss(fht_input[i],fht_input[FHT_SAMPLES_N - 1 - i]);
        fht_lin_out8[i] = rss_value > 255 ? 255 : rss_value;
    }
}



#endif //AVR_FHT_FHT_MAG_LIN8_NO_ASM_H
