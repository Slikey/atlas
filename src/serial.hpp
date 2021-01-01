#include <Arduino.h>

#define SERIAL_ENABLED
#define SERIAL_BAUD 2000000

#ifdef SERIAL_ENABLED
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_PRINTLN(...)
#define DEBUG_PRINT(...)
#define DEBUG_PRINTF(...)
#endif

class SerialModuleClass
{
private:
    uint32_t _next_debug_print;
    uint32_t _interval_debug_print = 60000;

public:
    void setup();
    void loop();
};

extern SerialModuleClass SerialModule;