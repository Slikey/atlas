#include "storage.hpp"

#include <EEPROM.h>

#define STORAGE_CURRENT_VERSION 0x10203041

void StorageModuleClass::setup()
{
    EEPROM.begin(sizeof(storage_data_t));
    EEPROM.get(0, _data);
    if (_data.version != STORAGE_CURRENT_VERSION)
    {
        memset(&_data, 0, sizeof(storage_data_t));
        _data.version = STORAGE_CURRENT_VERSION;
    }

    LittleFS.begin();
}

void StorageModuleClass::loop()
{
    // no need for dirty flag as EEPROM class handles that
    EEPROM.commit();
}

storage_data_t StorageModuleClass::get()
{
    return _data;
}

void StorageModuleClass::put(storage_data_t data)
{
    _data = data;
    EEPROM.put(0, data);
}

StorageModuleClass StorageModule;