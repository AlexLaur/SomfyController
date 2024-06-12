#include <DebugLog.h>
#include <ESP8266WiFi.h>

#include "wifiAccessPoint.h"


void WifiAccessPoint::startAccessPoint(const char *ssid, const char *password)
{
    LOG_DEBUG("Opening Access Point...");
    if (password != NULL){
        WiFi.softAP(ssid);
        LOG_DEBUG("Open Access Point (AP) started.");
    }
    else{
        WiFi.softAP(ssid, password);
        LOG_DEBUG("Access point with WPA2-PSK (AP) started.");
    }
};

IPAddress WifiAccessPoint::getIP()
{
    return WiFi.softAPIP();
};