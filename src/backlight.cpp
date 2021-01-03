#include "backlight.hpp"

#include "storage.hpp"
#include "wifi.hpp"

#define BACKLIGHT_PIN D4

uint8_t sat_addu8b(uint8_t x, uint8_t y)
{
    uint8_t res = x + y;
    res |= -(res < x);

    return res;
}

uint8_t sat_subu8b(uint8_t x, uint8_t y)
{
    uint8_t res = x - y;
    res &= -(res <= x);

    return res;
}

void BacklightModuleClass::setup()
{
    pinMode(BACKLIGHT_PIN, OUTPUT);
    setValue(0);

/*
    WebServer.on("/api/backlight/set", HTTP_POST, [&](AsyncWebServerRequest *request) {
        String valueArg = request->arg("value");
        uint8_t value = 0;
        if (valueArg)
        {
            value = valueArg.toInt();
        }

        setValue(value);
        request->send(200);
    });
    WebServer.on("/api/backlight/add", [&](AsyncWebServerRequest *request) {
        String valueArg = request->arg("value");
        if (valueArg)
        {
            addValue(value);
        }

        request->send(200);
    });
    */
}

void BacklightModuleClass::loop()
{
}

void BacklightModuleClass::setValue(uint8_t value)
{
    this->value = value;
    analogWrite(BACKLIGHT_PIN, value);
}

void BacklightModuleClass::addValue(uint8_t value)
{
    if (value == 0)
    {
        return;
    }
    setValue(sat_addu8b(this->value, value));
}

BacklightModuleClass BacklightModule;