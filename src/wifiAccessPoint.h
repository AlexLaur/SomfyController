#pragma once

#include <ESP8266WiFi.h>

class WifiAccessPoint
{
public:
    void startAccessPoint(const char *ssid, const char *password = NULL);
    IPAddress getIP();
};