#ifndef AVR_FHT_FHT_MAG_LIN8_NO_ASM_H
#define AVR_FHT_FHT_MAG_LIN8_NO_ASM_H

#include <FHT.h>

uint8_t __attribute__((used)) fht_lin_out8[FHT_AMPLITUDES_N]; // FHT linear output magintude buffer

const uint16_t _pow2_map[] PROGMEM = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
const uint8_t _rss_map[] PROGMEM = {7, 6, 6, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2};

#define ABS_UINT8_T(value) ((value) + ((value) >> 15)) ^ ((value) >> 15)

uint16_t fast_rss(int16_t a, int16_t b) {
    if (a == 0 and b == 0) return 0;

    if (a < 0) a = -a;
    if (b < 0) b = -b;
    int max = a > b ? a : b;
    int min = a > b ? b : a;

    if (max <= (min + min + min)) {
        int temp1 = min >> 3;
        if (temp1 == 0) temp1 = 1;
        int temp2 = min;
        uint8_t map_index = 0;
        while (temp2 < max) {
            temp2 += temp1;
            map_index++;
        }
        temp2 = _rss_map[map_index];
        temp1 = temp1 >> 1;
        for (int i = 0; i < temp2; i++) {
            max += temp1;
        }
    }
    return max;
}



void fht_mag_lin_no_asm() {

    for (int i = 0; i < FHT_AMPLITUDES_N; ++i) {
        int rss_value = fast_rss(fht_input[i],fht_input[FHT_SAMPLES_N - 1 - i]);
        fht_lin_out8[i] = rss_value;
    }
}



#endif //AVR_FHT_FHT_MAG_LIN8_NO_ASM_H
