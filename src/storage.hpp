#pragma once

#include <Arduino.h>
#include <LittleFS.h>

typedef struct
{
    uint32_t version;
} storage_data_t;

class StorageModuleClass
{
private:
    storage_data_t _data;

public:
    void setup();
    void loop();
    storage_data_t get();
    void put(storage_data_t data);
};

extern StorageModuleClass StorageModule;