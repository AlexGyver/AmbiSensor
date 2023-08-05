#pragma once
#include <Arduino.h>

#include "data.h"
#include "led.h"

#define GH_NO_MQTT
#include <GyverHub.h>
GyverHub hub("MyDevices", "AmbiSensor", "");

#include "multi_veml.h"
MultiVeml6040 rgb;

uint8_t matrix[8][2] = {
    {1, 2},
    {0, 2},
    {0, 1},
    {0, 0},
    {1, 0},
    {2, 0},
    {2, 1},
    {2, 2}};

void build() {
    hub.BeginWidgets();
    hub.WidgetSize(50);
    bool upd = false;
    upd |= hub.Input(&data.led_num[0], GH_UINT8, F("bottom left"));
    upd |= hub.Input(&data.led_num[1], GH_UINT8, F("left"));
    upd |= hub.Input(&data.led_num[2], GH_UINT8, F("top"));
    upd |= hub.Input(&data.led_num[3], GH_UINT8, F("right"));
    upd |= hub.Input(&data.led_num[4], GH_UINT8, F("bottom right"));
    if (upd) {
        memory.update();
        led_change();
    }

    hub.WidgetSize(100);
    if (hub.Slider(&data.led_br, GH_UINT8, F("bright"), 0, 255)) {
        memory.update();
    }

    hub.Canvas_("rgb", 300, 300, 0, 0, F("status"));

    if (hub.Button(0, F("calibrate"))) {
        for (int i = 0; i < 8; i++) {
            rgb.get(i).calibrate();
        }
        memory.update();
    }

    hub.WidgetSize(50);
    upd = false;
    upd |= hub.Spinner(&data.lr, GH_FLOAT, F("lut r"), 0, 2, 0.1);
    upd |= hub.Spinner(&data.lg, GH_FLOAT, F("lut g"), 0, 2, 0.1);
    upd |= hub.Spinner(&data.lb, GH_FLOAT, F("lut b"), 0, 2, 0.1);
    upd |= hub.Spinner(&data.ampli, GH_FLOAT, F("amplify"), 0, 2, 0.1);
    upd |= hub.Spinner(&data.sat, GH_FLOAT, F("saturate"), 0, 5, 0.1);
    if (upd) memory.update();
}

void hub_init() {
    hub.onBuild(build);
    hub.begin();

    Wire.begin();
    rgb.begin();
    for (int i = 0; i < 8; i++) {
        rgb.get(i).attachCalibrate(&data.calibr[i].r, &data.calibr[i].g, &data.calibr[i].b);
    }
}

void show_ambi() {
    static GHtimer tmr(45);
    if (tmr) {
        for (uint8_t i = 0; i < 5; i++) {
            if (!data.led_num[i]) return;
        }

        CRGB colors[8];
        for (uint8_t i = 0; i < 8; i++) {
            
            int r = rgb.get(i).getRed() * data.ampli * data.lr;
            int g = rgb.get(i).getGreen() * data.ampli * data.lg;
            int b = rgb.get(i).getBlue() * data.ampli * data.lb;

            r = min(r, 255);
            g = min(g, 255);
            b = min(b, 255);

            if (data.sat != 0) {
                // https://stackoverflow.com/a/34183839
                float gray = 0.2989 * r + 0.5870 * g + 0.1140 * b;
                r += (r - gray) * data.sat;
                g += (g - gray) * data.sat;
                b += (b - gray) * data.sat;

                r = constrain(r, 0, 255);
                g = constrain(g, 0, 255);
                b = constrain(b, 0, 255);
            }

            colors[i] = CRGB(r, g, b);
        }

        uint16_t led_to = 0;
        fill_gradient_RGB(leds, led_to, colors[0], (led_to += data.led_num[0] / 1) - 1, colors[1]);
        fill_gradient_RGB(leds, led_to, colors[1], (led_to += data.led_num[1] / 2) - 1, colors[2]);
        fill_gradient_RGB(leds, led_to, colors[2], (led_to += data.led_num[1] / 2) - 1, colors[3]);
        fill_gradient_RGB(leds, led_to, colors[3], (led_to += data.led_num[2] / 2) - 1, colors[4]);
        fill_gradient_RGB(leds, led_to, colors[4], (led_to += data.led_num[2] / 2) - 1, colors[5]);
        fill_gradient_RGB(leds, led_to, colors[5], (led_to += data.led_num[3] / 2) - 1, colors[6]);
        fill_gradient_RGB(leds, led_to, colors[6], (led_to += data.led_num[3] / 2) - 1, colors[7]);
        fill_gradient_RGB(leds, led_to, colors[7], (led_to += data.led_num[4] / 1) - 1, colors[0]);
        led_show();
    }
}

void send_colors() {
    static GHtimer tmr(100);
    if (tmr && hub.focused()) {
        GHcanvas cv;
        hub.sendCanvasBegin("rgb", cv);
        cv.noStroke();
        for (uint8_t i = 0; i < 8; i++) {
            cv.fill(rgb.get(i).getRGB());
            cv.rect(matrix[i][0] * 100, matrix[i][1] * 100, 100, 100);
        }
        hub.sendCanvasEnd(cv);
    }
}

void hub_tick() {
    memory.tick();
    hub.tick();
    send_colors();
    show_ambi();
}
