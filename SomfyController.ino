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

#include "default_config.h"
#include "src/wifiClient.h"
#include "src/eepromDatabase.h"

#include "src/dto/remote.h"
#include "src/dto/networks.h"

EEPROMDatabase database;
WifiClient wifiClient;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    continue;

  // Wait one second to avoid bad chars in serial
  delay(1000);

  // Database Setup
  LOG_INFO("Initializing database...");
  database.init();

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

  char* ssid = "";
  char* password = "";
  LOG_INFO("Trying WiFi connection...");
  // bool wifiConnected = wifiClient.connect(ssid, password);
  bool wifiConnected = false;
  if (!wifiConnected)
  {
    LOG_ERROR("Failed to connect to the WiFi.");
    // Not connected to wifi, starting the AP mode
    // LOG_INFO("Configuring access point (AP)...");
    // wifiAccessPoint.startAccessPoint(APSSID);
    // LOG_INFO("AP IP address:", wifiAccessPoint.getIP().toString());
    Network networks[MAX_NETWORK_SCAN];
    wifiClient.getNetworks(networks);
    for (int i = 0; i < sizeof(networks) / sizeof(Network); i++) {
      LOG_INFO(networks[i].SSID);
      LOG_INFO(networks[i].RSSI);
    }
    // free(networks);
  }
  else
  {
    LOG_INFO("WiFi IP address:", wifiClient.getIP().toString());
  }

  // TEST
  // LOG_INFO("TEST CONFIG HERE");
  // NetworkConfiguration conf = database.getNetworkConfiguration();
  // LOG_INFO(conf.ssid);
  // LOG_INFO(conf.password);

  // strcpy(conf.ssid, "foo");
  // strcpy(conf.password, "bar");

  // database.setNetworkConfiguration(conf);

  // NetworkConfiguration newConf = database.getNetworkConfiguration();
  // LOG_INFO(newConf.ssid);
  // LOG_INFO(newConf.password);

  LOG_INFO("TEST REMOTES HERE");

  // database.addRemote("foo");
  Remote remote = database.getRemote(REMOTE_BASE_ADDRESS + 10);
  remote.rolling_code += 1;
  database.updateRemote(remote);
  // LOG_INFO(remoteb.id);
  // LOG_INFO(remoteb.name);
  // LOG_INFO(remoteb.rolling_code);

  // Remote remotesArray[MAX_REMOTES];
  // Vector<Remote> remotesContainer(remotesArray);
  // database.getRemotes(remotesContainer);
  // for (auto element : remotesContainer){
  //   LOG_INFO(element.id);
  //   LOG_INFO(element.name);
  // }



  LOG_INFO("Print all remotes");
  Remote remotes[MAX_REMOTES];
  database.getAllRemotes(remotes);
  for (int i = 0; i < sizeof(remotes) / sizeof(Remote); i++)
  {
    LOG_INFO(remotes[i].id);
    LOG_INFO(remotes[i].rolling_code);
    LOG_INFO(remotes[i].name);
  }
};

void loop() {};