#include <Arduino.h>

class BacklightModuleClass
{
private:
    uint8_t value;

public:
    void setup();
    void loop();
    void setValue(uint8_t value);
    void addValue(uint8_t value);
};

extern BacklightModuleClass BacklightModule;