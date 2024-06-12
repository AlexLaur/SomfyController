#pragma once

#include <ESP8266WiFi.h>
#include "abstracts/networkClient.h"
#include "dto/networks.h"

class WifiClient: public NetworkClient
{
public:
    bool connect(const NetworkConfiguration &conf);
    bool connect(const char *ssid, const char *password);
    IPAddress getIP();
    bool isConnected();
    void getNetworks(Network networks[]);
};