#pragma once
#include <Arduino.h>

#include "veml6040.h"
#define TCA_ADDR 0x70
#define VEML_AMOUNT 8

class MultiVeml6040 {
   public:
    void begin() {
        for (uint8_t i = 0; i < VEML_AMOUNT; i++) {
            get(i).begin();
        }
    }
    VEML6040& get(uint8_t i) {
        _select(i);
        return rgb[i];
    }

    VEML6040 rgb[VEML_AMOUNT];

   private:
    void _select(uint8_t ch) {
        Wire.beginTransmission(TCA_ADDR);
        Wire.write(1 << ch);
        Wire.endTransmission();
    }
};