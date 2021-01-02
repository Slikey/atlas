// Libraries
#include "serial.hpp"
#include "web.hpp"
#include "storage.hpp"
#include "wifi.hpp"
#include "backlight.hpp"

void setup()
{
  SerialModule.setup();
  StorageModule.setup();

  WebServer.serveStatic("/static/", LittleFS, "/static/");

  WiFiModule.setup();
  BacklightModule.setup();
  WebServer.begin();
}

void loop()
{
  SerialModule.loop();
  StorageModule.loop();
  WiFiModule.loop();
  BacklightModule.loop();
}
