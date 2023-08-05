#include <Arduino.h>

#include "config.h"
#include "hub.h"
#include "led.h"

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(AP_SSID, AP_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println(WiFi.localIP());

    EEPROM.begin(memory.blockSize());
    memory.begin(0, 'b');

    hub_init();
    led_init();
    led_change();
}

void loop() {
    hub_tick();
}