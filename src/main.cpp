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
