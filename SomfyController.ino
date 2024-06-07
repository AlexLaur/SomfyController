/**
 * @file SomfyController.ino
 * @author Laurette Alexandre
 * @brief Control SOMFY blinds.
 * @version 2.0.0
 * @date 2024-06-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#define DEBUGLOG_DEFAULT_LOG_LEVEL_DEBUG

#include <DebugLog.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "src/wifiClient.h"
#include "src/database.h"

Database database;
WifiClient wifiClient;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    continue;

  // Wait one second to avoid bad chars in serial
  delay(1000);

  // SPIFFS Setup
  LOG_INFO("Setuping SPIFFS...");
  if (!LittleFS.begin())
  {
    LOG_ERROR("An Error has occurred while mounting SPIFFS.");
    return;
  }
  else
  {
    LOG_INFO("SPIFFS setup done.");
  }

  char *ssid = "";
  char *password = "";
  LOG_INFO("Trying WiFi connection...");
  bool wifiConnected = wifiClient.connect(ssid, password);
  if (!wifiConnected)
  {
    LOG_ERROR("Failed to connect to the WiFi.");
    // Not connected to wifi, starting the AP mode
    // LOG_INFO("Configuring access point (AP)...");
    // wifiAccessPoint.startAccessPoint(APSSID);
    // LOG_INFO("AP IP address:", wifiAccessPoint.getIP().toString());
    auto networks = wifiClient.getNetworks();
    for (int i = 0; i < sizeof(networks); i++) {
      LOG_INFO(networks[i].SSID);
      LOG_INFO(networks[i].RSSI);
    }
    free(networks);
  }
  else
  {
    LOG_INFO("WiFi IP address:", wifiClient.getIP().toString());
  }
};

void loop() {};