#pragma once

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include "web.hpp"
#include <set>

class WiFiModuleClass
{
private:
    uint32_t _time_next_scan;
    WiFiEventHandler _gotIpEventHandler, _disconnectedEventHandler;
    DNSServer _dns_server;

    // This flag turns off WiFi in the next loop() call. We can't do so from the HTTP handler as it needs an intact WiFi connection before closing the request
    bool _queue_disconnect = false;
    bool _running;

    void _start();
    void _stop();
    void _loopScanNetworks();
    uint8_t _getQualityFromRSSI(int32_t);

public:
    void setup();
    void loop();
};

extern WiFiModuleClass WiFiModule;