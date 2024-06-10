#pragma once

struct Network
{
    char SSID[32];
    int RSSI;
};


struct NetworkConfiguration
{
  char ssid[32];
  char password[63];
};