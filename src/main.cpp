#include <Arduino.h>
#include <LittleFS.h>

#include "wifi.hpp"
#include "backlight.hpp"
#include <map>

#define SERIAL_BAUD 2000000

std::map<String, String> string_maps = {
    {"title", "Atlas"},
    {"home", "Home"},
    {"metrics", "Metrics"},
    {"settings", "Settings"}};

WiFiModuleClass wifi("Atlas", Serial, LittleFS);

String processor_portal(const String &var)
{
  const String &s = string_maps[var];
  return s ? s : String();
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
