#ifndef AVR_FHT_FAST_MATH_H
#define AVR_FHT_FAST_MATH_H

#include <Arduino.h>

const uint8_t _rss_map[] PROGMEM = {7, 6, 6, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2};

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
        temp2 = map_index < 20 ? pgm_read_byte(&_rss_map[map_index]) : 1;
        temp1 >>= 1;
        for (int i = 0; i < temp2; i++) max += temp1;
    }
    return max;
}

#endif //AVR_FHT_FAST_MATH_H
