#include "wifi.hpp"

#include "storage.hpp"
#include "serial.hpp"
#include <ArduinoJson.h>

#define SETUP_DNS_PORT 53
#define SETUP_AP_SSID "StarSky Setup"
#define SETUP_AP_IP 172, 0, 0, 1
#define SETUP_AP_SUBNET 255, 255, 255, 0
#define SETUP_WIFI_SCAN_INTERVAL 30000
#define SETUP_NETWORKS_COUNT 10

void WiFiModuleClass::setup()
{
    // start WiFi
    WiFi.persistent(true);
    WiFi.mode(WIFI_STA);

    // fired when we successfully connected to a wifi network and received an IP address
    _gotIpEventHandler = WiFi.onStationModeGotIP([&](const WiFiEventStationModeGotIP &event) {
        Serial.print(F("Got IP: "));
        Serial.print(WiFi.localIP());
        Serial.println();

        _stop();
    });
    // fired when we got disconnected from a wifi network
    _disconnectedEventHandler = WiFi.onStationModeDisconnected([&](const WiFiEventStationModeDisconnected &event) {
        _start();
    });

    // next scan is now
    _time_next_scan = millis();

    // connection endpoint to accept a connection attempt, can only be called from captive portal
    WebServer.on("/wifi/connect", HTTP_GET, [this](AsyncWebServerRequest *request) {
                 // parse request data
                 String ssidArg = request->arg(F("ssid")); //"Honigtopf";
                 String passArg = request->arg(F("pass")); //"21356775765776304081";
                 if (!ssidArg)
                 {
                     request->send(400);
                     return;
                 }

                 const char *ssid = ssidArg.c_str();
                 const char *pass = passArg ? passArg.c_str() : "";

                 // attempt to connect
                 if (WiFi.begin(ssid, pass) == WL_CONNECT_FAILED)
                 {
                     request->send(400);
                 }
                 else
                 {
                     request->send(200);
                 }
             })
        .setFilter(ON_AP_FILTER);
    WebServer.on("/wifi/networks", HTTP_GET, [this](AsyncWebServerRequest *request) {
                 StaticJsonDocument<128 * (SETUP_NETWORKS_COUNT + 1)> doc;
                 JsonArray networks = doc.createNestedArray("networks");

                 int8_t n = WiFi.scanComplete();
                 if (n >= 0)
                 {
                     if (n > SETUP_NETWORKS_COUNT)
                         n = SETUP_NETWORKS_COUNT;
                     for (int i = 0; i < n; i++)
                     {
                         JsonObject networkJson = networks.createNestedObject();
                         networkJson["ssid"] = WiFi.SSID(i);
                         networkJson["quality"] = _getQualityFromRSSI(WiFi.RSSI(i));
                         networkJson["encrypted"] = WiFi.encryptionType(i) != ENC_TYPE_NONE;
                     }
                 }

                 String s;
                 serializeJson(doc, s);
                 request->send(200, F("application/json"), s);
             })
        .setFilter(ON_AP_FILTER);
    // disconnect can only be called from operation mode
    WebServer.on("/wifi/disconnect", HTTP_GET, [this](AsyncWebServerRequest *request) {
                 _queue_disconnect = true;

                 request->send(200, F("text/plain"), F("ok"));
             })
        .setFilter(ON_STA_FILTER);
    // captive portal only responds in AP wifi connections
    WebServer.on("*", HTTP_GET, [](AsyncWebServerRequest *request) {
                 request->send(LittleFS, F("/portal.html"), F("text/html"), false);
             })
        .setFilter(ON_AP_FILTER);

    // connect to existing WiFi or start AP
    String ssid = WiFi.SSID();
    if (!ssid.isEmpty())
    {
        DEBUG_PRINT(F("Connecting to WiFi: "));
        DEBUG_PRINT(ssid);
        DEBUG_PRINTLN();
        wl_status_t status = WiFi.begin();
        DEBUG_PRINT(F("            Status:"));
        DEBUG_PRINT(status);
        DEBUG_PRINTLN();
        if (status == WL_CONNECT_FAILED)
        { // illegal data
            _start();
        }
    }
    else
    {
        _start();
    }
}

void WiFiModuleClass::loop()
{
    if (_queue_disconnect)
    {
        _queue_disconnect = false;
        WiFi.disconnect(false);
    }

    // call start
    if (!_running)
    {
        return;
    }

    // dns spoof
    _dns_server.processNextRequest();

    _loopScanNetworks();
}

void WiFiModuleClass::_loopScanNetworks()
{
    // update scanned networks
    if (_time_next_scan < millis())
    {
        WiFi.scanNetworks(true);
        _time_next_scan = millis() + SETUP_WIFI_SCAN_INTERVAL;
    }
}

void WiFiModuleClass::_start()
{
    if (_running)
    { // do not start services if they are started
        return;
    }

    Serial.println(F("Starting setup!"));

    const IPAddress wifi_ap_ip(SETUP_AP_IP);
    const IPAddress wifi_ap_gateway(SETUP_AP_IP);
    const IPAddress wifi_ap_subnet(SETUP_AP_SUBNET);

    _dns_server.start(SETUP_DNS_PORT, F("*"), wifi_ap_ip);

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(wifi_ap_ip, wifi_ap_gateway, wifi_ap_subnet);
    WiFi.softAP(F(SETUP_AP_SSID));

    _running = true;
}

void WiFiModuleClass::_stop()
{
    if (!_running)
    { // do not stop services if they are not started
        return;
    }

    _running = false;

    WiFi.softAPdisconnect();
    WiFi.scanDelete();
    WiFi.mode(WIFI_STA);

    _dns_server.stop();

    Serial.println(F("Stopped setup!"));
}

uint8_t WiFiModuleClass::_getQualityFromRSSI(int32_t rssi)
{
    int quality = 0;

    if (rssi <= -100)
    {
        quality = 0;
    }
    else if (rssi >= -50)
    {
        quality = 100;
    }
    else
    {
        quality = 2 * (rssi + 100);
    }
    return quality;
}

WiFiModuleClass WiFiModule;