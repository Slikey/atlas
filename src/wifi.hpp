#pragma once

#include <ESP8266WiFi.h>
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

#ifndef SETUP_NETWORKS_COUNT
#define SETUP_NETWORKS_COUNT 10
#endif

class WiFiModuleClass
{
private:
    String _ap_ssid;
    Print &_print;
    FS &_fs;

    IPAddress _ap_ip, _ap_gateway, _ap_subnet;
    WiFiEventHandler _gotIpEventHandler, _disconnectedEventHandler;

    DNSServer _dns_server;
    uint16_t _dns_port;

    AsyncWebServer _web_server;

    uint32_t _time_scan_interval;
    uint32_t _time_next_scan;

    // This flag turns off WiFi in the next loop() call. We can't do so from the HTTP handler as it needs an intact WiFi connection before closing the request
    bool _queue_disconnect = false;
    bool _running;

    void _start();
    void _stop();
    void _loopScanNetworks();
    uint8_t _getQualityFromRSSI(int32_t);

public:
    WiFiModuleClass(String ap_ssid, Print &print, fs::FS &fs);
    void setup();
    void loop();
};