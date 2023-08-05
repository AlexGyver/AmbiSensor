#pragma once
#include <Arduino.h>
#include <EEManager.h>

struct Data {
    struct rgb_cal {
        uint16_t r = 0, g = 0, b = 0;
    };
    float sat = 0;
    float ampli = 1;
    float lr = 1, lg = 1, lb = 1;
    rgb_cal calibr[8];
    uint8_t led_num[5] = {};
    uint8_t led_br = 255;
};

Data data;
EEManager memory(data);