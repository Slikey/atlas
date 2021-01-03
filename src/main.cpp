// Libraries
#include "serial.hpp"
#include "storage.hpp"
#include "wifi.hpp"
#include "backlight.hpp"

WiFiModuleClass WiFiModule("Atlas", Serial, LittleFS);

void setup()
{
  SerialModule.setup();
  StorageModule.setup();

  WiFiModule.setup();
  BacklightModule.setup();
}

void loop()
{
  SerialModule.loop();
  StorageModule.loop();
  WiFiModule.loop();
  BacklightModule.loop();
}
