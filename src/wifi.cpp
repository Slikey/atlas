#include "wifi.hpp"

#include <ArduinoJson.h>

#define SETUP_AP_IP 172, 0, 0, 1
#define SETUP_AP_GATEWAY SETUP_AP_IP
#define SETUP_AP_SUBNET 255, 255, 255, 0
#define SETUP_DNS_PORT 53
#define SETUP_WIFI_SCAN_INTERVAL 10000

#define SETUP_HTTP_PORT 80
#define SETUP_HTTP_CACHE_CONTROL "max-age=31536000"

WiFiModuleClass::WiFiModuleClass(String ap_ssid,
                                 Print &print,
                                 fs::FS &fs) : _ap_ssid(ap_ssid),
                                               _print(print),
                                               _fs(fs),
                                               _ap_ip(SETUP_AP_IP),
                                               _ap_gateway(SETUP_AP_GATEWAY),
                                               _ap_subnet(SETUP_AP_SUBNET),
                                               _dns_port(SETUP_DNS_PORT),
                                               _web_server(SETUP_HTTP_PORT),
                                               _time_scan_interval(SETUP_WIFI_SCAN_INTERVAL)
{
}

void WiFiModuleClass::setup()
{
    // start WiFi
    WiFi.persistent(true);
    WiFi.mode(WIFI_STA);

    // next scan is now
    _time_next_scan = millis();

    // fired when we successfully connected to a wifi network and received an IP address
    _gotIpEventHandler = WiFi.onStationModeGotIP([&](const WiFiEventStationModeGotIP &event) {
        _print.print(F("Got IP: "));
        _print.print(WiFi.localIP());
        _print.println();

        _stop();
    });
    // fired when we got disconnected from a wifi network
    _disconnectedEventHandler = WiFi.onStationModeDisconnected([&](const WiFiEventStationModeDisconnected &event) {
        _start();
    });

    // POST endpoint to accept a connection attempt, can only be called from captive portal
    _web_server.on("/wifi/connect", HTTP_POST, [this](AsyncWebServerRequest *request) {
                   // parse request data
                   String ssidArg = request->arg(F("ssid"));
                   String passArg = request->arg(F("pass"));

                   if (!ssidArg)
                   {
                       request->send(400);
                       return;
                   }

                   const char *ssid = ssidArg.c_str();
                   const char *pass = passArg ? passArg.c_str() : nullptr;
                   _print.printf("Connection Request:\n\tSSID: %s\n\tPass: %s\n", ssid, pass);

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

    // GET endpoint to discover wifi networks in range
    _web_server.on("/wifi/networks", HTTP_GET, [this](AsyncWebServerRequest *request) {
                   // TODO: Add pagination for more than SETUP_NETWORKS_COUNT results
                   StaticJsonDocument<128 * (SETUP_NETWORKS_COUNT + 1)> doc;
                   JsonArray networks = doc.createNestedArray("networks");

                   int8_t n = WiFi.scanComplete();
                   if (n > SETUP_NETWORKS_COUNT)
                       n = SETUP_NETWORKS_COUNT;
                   for (int i = 0; i < n; i++)
                   {
                       JsonObject networkJson = networks.createNestedObject();
                       networkJson["ssid"] = WiFi.SSID(i);
                       networkJson["quality"] = _getQualityFromRSSI(WiFi.RSSI(i));
                       networkJson["encrypted"] = WiFi.encryptionType(i) != ENC_TYPE_NONE;
                   }

                   String s;
                   serializeJson(doc, s);
                   request->send(200, F("application/json"), s); // experience is this takes 2ms until here
               })
        .setFilter(ON_AP_FILTER);

    // disconnect can only be called from operation mode
    _web_server.on("/wifi/disconnect", HTTP_GET, [this](AsyncWebServerRequest *request) {
                   _queue_disconnect = true;

                   request->send(200, F("text/plain"), F("ok"));
               })
        .setFilter(ON_STA_FILTER);

    // captive portal only responds in AP wifi connections
    _web_server.serveStatic("/static/", _fs, "/static/", SETUP_HTTP_CACHE_CONTROL);
    _web_server.serveStatic("/", _fs, "/ap/", SETUP_HTTP_CACHE_CONTROL).setFilter(ON_AP_FILTER);
    _web_server.serveStatic("/", _fs, "/sta/", SETUP_HTTP_CACHE_CONTROL).setFilter(ON_STA_FILTER);
    _web_server.rewrite("/", "/index.html");

    // redirect to /portal.html when called from AP to create captive portal
    _web_server.onNotFound([this](AsyncWebServerRequest *request) {
        if (request->client()->localIP() == _ap_ip)
        {
            request->redirect("/index.html");
            return;
        }

        request->send(404, F("text/plain"), F("not found"));
    });

    // start webserver
    _web_server.begin();

    // connect to existing WiFi or start AP
    String ssid = WiFi.SSID();
    if (!ssid.isEmpty())
    {
        _print.printf("Connecting to WiFi: %s\n", ssid.c_str());
        wl_status_t status = WiFi.begin();
        _print.printf("            Status: %u\n", status);
        if (status == WL_CONNECT_FAILED)
        { // illegal data stored or non at all
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
        _time_next_scan = millis() + _time_scan_interval;
    }
}

void WiFiModuleClass::_start()
{
    if (_running)
    { // do not start services if they are started
        return;
    }

    _dns_server.start(_dns_port, F("*"), _ap_ip);

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(_ap_ip, _ap_gateway, _ap_subnet);
    WiFi.softAP(_ap_ssid);

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

    _print.println(F("Stopped setup!"));
}

uint8_t WiFiModuleClass::_getQualityFromRSSI(int32_t rssi)
{
    if (rssi <= -100)
    {
        return 0;
    }
    else if (rssi >= -50)
    {
        return 100;
    }
    else
    {
        return 2 * (rssi + 100);
    }
}

AsyncWebServer &WiFiModuleClass::getWebServer()
{
    return _web_server;
}