// reorder table creator
// guest openmusiclabs.com 8.12.12
// creates a list of swap pairs for fft sample reordering
// prints out the pairs to the serial monitor
// thanks to smark for helping me figure this out

#define SIZE 128 // fft size [256, 128, 64, 32, 16]

#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial.println();
}

[[noreturn]] void loop() {

#if (SIZE == 256)
    for (uint8_t i = 0x0f ; i > 0; i--) {
        for (uint8_t j = 0x00 ; j < i ; j++) {
            uint8_t k = (i << 4) | j;
            // reverser function
            uint8_t l = ((k >> 1) & 0x55) | ((k << 1) & 0xaa);
            l = ((l >> 2) & 0x33) | ((l << 2) & 0xcc);
            uint8_t m = ((k & 0x0f0) | (l & 0x0f));
            l = ((l >> 4) & 0x0f) | ((k << 4) & 0xf0);
#elif ((SIZE == 64) || (SIZE == 128))
    for (uint8_t i = 0x07 ; i > 0; i--) {
        for (uint8_t j = 0x00 ; j < i ; j++) {
            uint8_t k = (i << 3) | j;
            // reverser function
            uint8_t l = ((k >> 2) & 0x09) | ((k << 2) & 0x24) | (k & 0x12);
  #if (SIZE ==  64)
            uint8_t m = ((k & 0x38) | (l & 0x07));
            l = ((l >> 3) & 0x07) | ((k << 3) & 0x38);
  #elif (SIZE == 128)
            uint8_t m = (((k << 1) & 0x70) | (l & 0x07));
            l = ((l >> 3) & 0x07) | ((k << 4) & 0x70);
            Serial.print(m,DEC);
            Serial.print(", \t");
            Serial.print(l,DEC);
            Serial.println(",");
            m |= 0x08;
            l |= 0x08;
  #endif
#elif ((SIZE == 16) || (SIZE == 32))
    for (uint8_t i = 0x03 ; i > 0; i--) {
        for (uint8_t j = 0x00 ; j < i ; j++) {
            uint8_t k = (i << 2) | j;
            // reverser function
            uint8_t l = ((k >> 1) & 0x05) | ((k << 1) & 0x0a);
  #if (SIZE == 16)
            uint8_t m = ((k & 0x0c) | (l & 0x03));
            l = ((l >> 2) & 0x03) | ((k << 2) & 0x0c);
  #elif (SIZE == 32)
            uint8_t m = (((k << 1) & 0x18) | (l & 0x03));
            l = ((l >> 2) & 0x03) | ((k << 3) & 0x18);
            Serial.print(m,DEC);
            Serial.print(", \t");
            Serial.print(l,DEC);
            Serial.println(",");
            m |= 0x04;
            l |= 0x04;
  #endif
#endif
            Serial.print(m,DEC);
            Serial.print(", \t");
            Serial.print(l,DEC);
            Serial.println(",");
        }
    }
    while (true); // stop after list is created
}
