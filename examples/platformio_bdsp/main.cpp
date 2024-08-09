#define GAIN_MIC_PIN A4
#define AR_MIC_PIN A5
#define MAX9814_PIN A2
#define SAMPLES_OFFSET 575
#define SAMPLES_READ_DELAY_US 0

#include <Arduino.h>
#include "BDSP.h"
#include "AvrFHT.h"
#include "fht_window.h"
#include "fht_reorder.h"
#include "fht_mag_lin8_no_asm.h"

BDSPTransmitter transmitter;

void setup() {
    Serial.begin(SERIAL_SPEED);
    Serial.println();

    COBS::config_t config = {.delimiter = '\n', .depth = 255};
    transmitter.set_config(config, [] (uint8_t *data_p, size_t size, void *context) {
        Serial.write(data_p, size);
        Serial.flush();
    });

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
    transmitter.send_data(2, fht_lin_out8, FHT_AMPLITUDES_N);
}