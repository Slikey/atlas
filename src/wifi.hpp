#pragma once

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>

#define HTTP_CACHE_CONTROL "max-age=31536000"

class WiFiModuleClass
{
private:
    String _ap_ssid;
    IPAddress _ap_ip, _ap_gateway, _ap_subnet;
    uint16_t _dns_port;

    AsyncWebServer _web_server;
    WiFiEventHandler _gotIpEventHandler, _disconnectedEventHandler;
    DNSServer _dns_server;

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
    WiFiModuleClass(String ap_ssid);
    void setup();
    void loop();
};