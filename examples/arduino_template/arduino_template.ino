#define GAIN_MIC_PIN A4
#define AR_MIC_PIN A5
#define MAX9814_PIN A2
#define SAMPLES_OFFSET 575
#define SAMPLES_READ_DELAY_US 0

#include <Arduino.h>
#include "AvrFHT.h"
#include "fht_window.h"
#include "fht_reorder.h"
#include "fht_mag_lin8_no_asm.h"

void setup() {
    Serial.begin(115200);
    Serial.println();

    pinMode(AR_MIC_PIN, OUTPUT);
    digitalWrite(AR_MIC_PIN, false);
    pinMode(GAIN_MIC_PIN, OUTPUT);
    digitalWrite(GAIN_MIC_PIN, true);
    analogReference(EXTERNAL);
    _SFR_BYTE(ADCSRA) &= ~_BV(ADPS0);

}

void loop() {
    for (int & sample : fht_input) {
        sample = analogRead(MAX9814_PIN) - SAMPLES_OFFSET;
        delayMicroseconds(SAMPLES_READ_DELAY_US);
    }
    fht_window();
    fht_reorder();
    fht_transform();
    fht_mag_lin8_no_asm();

    Serial.print(fht_lin_out8[0]);
    for (int i = 1; i < FHT_AMPLITUDES_N; ++i) {
        Serial.print(',');
        Serial.print(fht_lin_out8[i]);
    }
    Serial.println();
}