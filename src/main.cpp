#include <Arduino.h>
#include <LittleFS.h>

#include "wifi.hpp"
#include "backlight.hpp"

#define SERIAL_BAUD 2000000

WiFiModuleClass wifi("Atlas", Serial, LittleFS);

String processor_portal(const String &var)
{
  if (var == "HELLO_FROM_TEMPLATE")
    return F("Hello world!");
  return String();
}

void setup()
{
  Serial.begin(SERIAL_BAUD);
  LittleFS.begin();
  wifi.setup();

  wifi.getWebServer().on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/panel.html", "text/html", false, processor_portal);
  });
}

void loop()
{
  wifi.loop();
}
