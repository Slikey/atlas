#include "serial.hpp"

void SerialModuleClass::setup()
{
#ifdef SERIAL_ENABLED
    Serial.begin(SERIAL_BAUD);
    while (!Serial)
    {
        yield();
    }
    Serial.println(); // empty line to jump from garbage line

    _next_debug_print = millis();
#endif
}

void SerialModuleClass::loop()
{
#ifdef SERIAL_ENABLED
    uint32_t ms = millis();
    if (_next_debug_print <= ms)
    {
        _next_debug_print = ms + _interval_debug_print;
        DEBUG_PRINTF("Free Heap: %u, Heap Fragmentation: %u\n", ESP.getFreeHeap(), ESP.getHeapFragmentation());
    }
#endif
}

SerialModuleClass SerialModule;