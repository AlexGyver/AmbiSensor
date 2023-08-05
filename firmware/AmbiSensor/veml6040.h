#pragma once
#include <Arduino.h>
#include <Wire.h>

#define VL_ADDRESS 0x10

#define VL_IT_40 0x0
#define VL_IT_80 0x1
#define VL_IT_160 0x2
#define VL_IT_320 0x3
#define VL_IT_640 0x4
#define VL_IT_1280 0x5

#define VL_RED 0x08
#define VL_GREEN 0x09
#define VL_BLUE 0x0A

class VEML6040 {
   public:
    bool begin() {
        Wire.beginTransmission(VL_ADDRESS);
        _ok = !Wire.endTransmission();
        if (_ok) config();
        return _ok;
    }

    bool state() {
        return _ok;
    }

    void config(uint8_t it = VL_IT_40, bool trig = 0, bool af = 0, bool sd = 0) {
        if (!_ok) return;
        Wire.beginTransmission(VL_ADDRESS);
        Wire.write(0x00);  // cmd
        Wire.write((it << 4) | (trig << 2) | (af << 1) | sd);
        Wire.write(0);
        Wire.endTransmission();
    }

    void attachCalibrate(uint16_t* r, uint16_t* g, uint16_t* b) {
        _maxr = r;
        _maxb = b;
        _maxg = g;
    }

    void calibrate() {
        if (!_maxr) return;
        *_maxr = _read(VL_RED);
        *_maxg = _read(VL_GREEN);
        *_maxb = _read(VL_BLUE);
    }

    // raw
    uint32_t getRGBRaw() {
        return ((uint32_t)_read(VL_RED) << 16) | (_read(VL_GREEN) << 8) | (_read(VL_BLUE) << 0);
    }
    uint16_t getRedRaw() {
        return _read(VL_RED);
    }
    uint16_t getGreenRaw() {
        return _read(VL_GREEN);
    }
    uint16_t getBlueRaw() {
        return _read(VL_BLUE);
    }

    // calibrated
    uint32_t getRGB() {
        return ((uint32_t)getRed() << 16) | (getGreen() << 8) | (getBlue() << 0);
    }
    uint8_t getRed() {
        return _getColor(VL_RED, _maxr);
    }
    uint8_t getGreen() {
        return _getColor(VL_GREEN, _maxg);
    }
    uint8_t getBlue() {
        return _getColor(VL_BLUE, _maxb);
    }

   private:
    uint16_t* _maxr = nullptr;
    uint16_t* _maxg = nullptr;
    uint16_t* _maxb = nullptr;
    bool _ok = 0;

    uint8_t _getColor(uint8_t col, uint16_t* cal) {
        if (!_ok) return 0;
        uint16_t v = _read(col);
        if (cal && *cal) v = (uint32_t)v * 255 / (*cal);
        if (v > 255) v = 255;
        return ((uint32_t)v * v + 255) >> 8;
    }
    uint16_t _read(uint8_t cmd) {
        if (!_ok) return 0;
        uint16_t data = 0;
        Wire.beginTransmission(VL_ADDRESS);
        Wire.write(cmd);
        Wire.endTransmission(0);

        Wire.requestFrom(VL_ADDRESS, 2);
        data = Wire.read();
        data |= Wire.read() << 8;
        return data;
    }
};