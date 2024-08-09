#ifndef AVR_FHT_H
#define AVR_FHT_H

#include "Arduino.h"
#include <avr/pgmspace.h>

#define STRINGIFY_(a) #a
#define STRINGIFY(a) STRINGIFY_(a)

#if not defined(FHT_N)
#define FHT_N 256
#endif

#if FHT_N == 256
#define LOG_N 8
#define _R_V 8 // reorder value - used for reorder list
#elif  FHT_N == 128
#define LOG_N 7
  #define _R_V 8
#elif FHT_N == 64
  #define LOG_N 6
  #define _R_V 4
#elif FHT_N == 32
  #define LOG_N 5
  #define _R_V 4
#elif FHT_N == 16
  #define LOG_N 4
  #define _R_V 2
#else
  #error FHT_N value not defined
#endif

int __attribute__((used)) fht_input[(FHT_N)]; // FHT input data buffer

extern const int16_t __attribute__((used)) _cas_constants[] PROGMEM = {
#if (FHT_N ==  256)
    #include <cas_lookup_256.inc>
#elif (FHT_N ==  128)
    #include <cas_lookup_128.inc>
#elif (FHT_N ==  64)
    #include <cas_lookup_64.inc>
#elif (FHT_N ==  32)
    #include <cas_lookup_32.inc>
#elif (FHT_N ==  16)
    #include <cas_lookup_16.inc>
#endif
};

#include "fht_transform.h"

#endif //AVR_FHT_H