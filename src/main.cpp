#include <Arduino.h>
#include <LittleFS.h>

#include "wifi.hpp"
#include "backlight.hpp"

#define SERIAL_BAUD 2000000

WiFiModuleClass WiFiModule("Atlas", Serial, LittleFS);

void setup()
{
  Serial.begin(SERIAL_BAUD, SERIAL_8N1, SERIAL_TX_ONLY);
  LittleFS.begin();
  WiFiModule.setup();
}

void loop()
{
  WiFiModule.loop();
}
