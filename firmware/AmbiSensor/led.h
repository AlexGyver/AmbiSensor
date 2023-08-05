#pragma once
#include <Arduino.h>
#include <FastLED.h>

#include "config.h"
#include "data.h"

CLEDController *cled = nullptr;
CRGB *leds = nullptr;
uint16_t led_am;

void led_init() {
    leds = (CRGB *)malloc(sizeof(CRGB));
    cled = &FastLED.addLeds<LED_CHIP, LED_PIN, LED_ORDER>(leds, 0).setCorrection(TypicalLEDStrip);
}

uint16_t led_amount() {
    return led_am;
}

void led_change() {
    led_am = 0;
    for (int i = 0; i < 5; i++) led_am += data.led_num[i];
    leds = (CRGB *)realloc(leds, sizeof(CRGB) * led_am);
    cled->setLeds(leds, led_am);
}

void led_show() {
    cled->showLeds(data.led_br);
}