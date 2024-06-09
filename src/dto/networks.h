#pragma once

struct Network
{
    String SSID;
    long RSSI;
};


struct NetworkConfiguration
{
  char ssid[32];
  char password[63];
};