#pragma once

struct Network
{
    String SSID;
    long RSSI;
};


struct NetworkConfiguration
{
  char ssid[30];
  char password[30];
};