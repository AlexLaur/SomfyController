#pragma once

#include <ESP8266WiFi.h>
#include "dto/networks.h"

class WifiClient
{
public:
    bool connect(const NetworkConfiguration &conf);
    bool connect(const char *ssid, const char *password);
    IPAddress getIP();
    bool isConnected();
    Network* getNetworks();

};