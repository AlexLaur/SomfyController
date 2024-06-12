#pragma once

#include <ESP8266WiFi.h> // TODO: Should be removed from here
#include "../dto/networks.h"

class NetworkClient
{
public:
    virtual bool connect(const NetworkConfiguration &conf) = 0;
    virtual bool connect(const char *ssid, const char *password) = 0;
    virtual IPAddress getIP() = 0;  // TODO: to specific to WIFI, should be changed
    virtual bool isConnected() = 0;
    virtual void getNetworks(Network networks[]) = 0;
};