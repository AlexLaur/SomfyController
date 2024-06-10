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
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "default_config.h"
#include "src/wifiClient.h"
#include "src/wifiAccessPoint.h"
#include "src/eepromDatabase.h"

#include "src/dto/remote.h"
#include "src/dto/networks.h"

EEPROMDatabase database;
WifiClient wifiClient;
WifiAccessPoint wifiAP;
AsyncWebServer server(SERVER_PORT);

Network networks[MAX_NETWORK_SCAN];

// ============================================================================
// WEBSERVER CALLBACKS
// ============================================================================

void homePage(AsyncWebServerRequest* request) { request->send(LittleFS, "/wifi.html", String()); };

void fetchWifiNetworks(AsyncWebServerRequest* request)
{
  String output;
  JsonDocument doc;
  JsonArray array = doc.to<JsonArray>();
  for (int i = 0; i < MAX_NETWORK_SCAN; i++)
  {
    if (strlen(networks[i].SSID) == 0)
    {
      continue;
    }
    JsonObject object = array.createNestedObject();
    object["ssid"] = networks[i].SSID;
    object["rssi"] = networks[i].RSSI;
  }
  serializeJson(doc, output);
  request->send(200, "application/json", output);
};

void fetchAllRemotes(AsyncWebServerRequest* request)
{
  String output;
  JsonDocument doc;
  JsonArray array = doc.to<JsonArray>();

  Remote remotes[MAX_REMOTES];
  database.getAllRemotes(remotes);

  for (int i = 0; i < MAX_REMOTES; i++)
  {
    if (remotes[i].id == 0)
    {
      continue;
    }
    JsonObject object = array.createNestedObject();
    object["id"] = remotes[i].id;
    object["rolling_code"] = remotes[i].rollingCode;
    object["name"] = remotes[i].name;
  }
  serializeJson(doc, output);
  request->send(200, "application/json", output);
};

void createRemote(AsyncWebServerRequest* request, JsonVariant& json)
{
  LOG_INFO("Handle remote creation.");
  const JsonObject& data = json.as<JsonObject>();
  const char* name = data["name"];

  if (name == nullptr)
  {
    LOG_ERROR("The name of the remote should be specified.");
    request->send(
        400, "application/json", "{\"message\":\"The name of the remote should be specified.\"}");
    return;
  }
  if (strlen(name) == 0)
  {
    LOG_ERROR("The name of the remote cannot be empty.");
    request->send(
        400, "application/json", "{\"message\":\"The name of the remote cannot be empty.\"}");
    return;
  }

  Remote remote = database.createRemote(name);

  if (remote.id == 0)
  {
    LOG_ERROR("The created remote is an empty remote.");
    request->send(
        400, "application/json", "{\"message\":\"No space left on the device for a new remote.\"}");
    return;
  }

  String response;
  JsonDocument doc;
  JsonObject object = doc.to<JsonObject>();
  object["remote_id"] = remote.id;
  object["rolling_code"] = remote.rollingCode;
  object["name"] = remote.name;
  serializeJson(object, response);
  request->send(201, "application/json", response);
};

void deleteRemote(AsyncWebServerRequest* request, JsonVariant& json)
{
  LOG_INFO("Handle remote deletion.");
  const JsonObject& data = json.as<JsonObject>();
  const unsigned long id = data["remote_id"];
  if (id == 0)
  {
    LOG_ERROR("The remote id not specified.");
    request->send(400, "application/json", "{\"message\":\"The remote id should be specified.\"}");
    return;
  }
  bool result = database.deleteRemote(id);
  if (!result)
  {
    LOG_ERROR("The given remote doesn't exist in the database.");
    request->send(
        400, "application/json", "{\"message\":\"The remote with the given id doesn't exist.\"}");
    return;
  }
  request->send(200, "application/json", "{\"message\":\"The remote has been deleted.\"}");
}

void updateWifiConfiguration(AsyncWebServerRequest* request, JsonVariant& json)
{
  LOG_INFO("Handle update Wifi Configuration");
  const JsonObject& data = json.as<JsonObject>();
  const char* ssid = data["ssid"];
  const char* password = data["password"];

  if (ssid == nullptr)
  {
    LOG_ERROR("The ssid cannot be empty.");
    request->send(400, "application/json", "{\"message\":\"The ssid should be specified.\"}");
    return;
  }

  if (strlen(ssid) == 0)
  {
    LOG_ERROR("The ssid cannot be empty.");
    request->send(400, "application/json", "{\"message\":\"The ssid cannot be empty.\"}");
    return;
  }

  NetworkConfiguration networkConfig;
  strcpy(networkConfig.ssid, ssid);

  if (password == nullptr)
  {
    LOG_DEBUG("No password provided. Probably a free hotspot.");
  }
  else
  {
    strcpy(networkConfig.password, password);
  }

  database.setNetworkConfiguration(networkConfig);
  request->send(
      200, "application/json", "{\"message\":\"The WiFi configuration has been updated.\"}");
};

void updateRemote(AsyncWebServerRequest* request, JsonVariant& json)
{
  LOG_INFO("Handle update remote");
  const JsonObject& payload = json.as<JsonObject>();

  const unsigned long id = payload["remote_id"];

  if (id == 0)
  {
    LOG_ERROR("The remote id not specified.");
    request->send(400, "application/json", "{\"message\":\"The remote id should be specified.\"}");
    return;
  }

  const JsonObject data = payload["data"];

  const char* name = data["name"];
  // const unsigned int rollingCode = data["rolling_code"]; // Don't permit this for now.

  Remote remote = database.getRemote(id);
  if (remote.id == 0)
  {
    LOG_WARN("The remote doesn't exist. It cannot be updated.");
    request->send(400, "application/json",
        "{\"message\":\"The remote doesn't exist. It cannot be updated.\"}");
    return;
  }

  if (name != nullptr)
  {
    if (strlen(name) != 0)
    {
      strcpy(remote.name, name);
    }
  }

  bool result = database.updateRemote(remote);
  if (!result)
  {
    LOG_ERROR("Failed to update the remote.");
    request->send(400, "application/json",
        "{\"message\":\"Something went wrong while updating the remote.\"}");
    return;
  }
  request->send(200, "application/json", "{\"message\":\"The remote has been updated.\"}");
};

void actionRemote(AsyncWebServerRequest* request, JsonVariant& json)
{
  LOG_DEBUG("Handle action remote");
  const JsonObject& payload = json.as<JsonObject>();

  const unsigned long id = payload["remote_id"];
  const char* action = payload["action"];

  if (id == 0)
  {
    LOG_ERROR("The remote id is not specified.");
    request->send(400, "application/json", "{\"message\":\"The remote id should be specified.\"}");
    return;
  }

  if (action == nullptr)
  {
    LOG_ERROR("The action is not specified.");
    request->send(400, "application/json",
        "{\"message\":\"The action should be specified. Allowed actions: up, down, stop, pair, "
        "reset\"}");
    return;
  }

  Remote remote = database.getRemote(id);
  if (remote.id == 0)
  {
    LOG_WARN("The remote doesn't exist. No action to do.");
    request->send(
        400, "application/json", "{\"message\":\"The remote doesn't exist. No action to do.\"}");
    return;
  }

  if (strcmp(action, "up") == 0)
  {
  }
  else if (strcmp(action, "stop") == 0)
  {
  }
  else if (strcmp(action, "down") == 0)
  {
  }
  else if (strcmp(action, "pair") == 0)
  {
  }
  else if (strcmp(action, "reset") == 0)
  {
  }
  else
  {
    LOG_WARN("The action is not valid.");
    request->send(400, "application/json",
        "{\"message\":\"The action is not valid. Allowed actions: up, down, stop, pair, reset\"}");
    return;
  }

  request->send(200, "application/json", "{\"message\":\"OK\"}");
};

// ============================================================================
// SETUP
// ============================================================================

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

  // WIFI Setup
  LOG_INFO("Fetching all wifi networks...");
  wifiClient.getNetworks(networks);
  LOG_INFO("Trying WiFi connection...");
  NetworkConfiguration networkConfig = database.getNetworkConfiguration();
  if (strlen(networkConfig.ssid) == 0)
  {
    LOG_WARN("No wifi configuration found.");
    LOG_INFO("Configuring access point (AP)...");
    wifiAP.startAccessPoint(AP_SSID, AP_PASSWORD);
    LOG_INFO("AP IP address:", wifiAP.getIP().toString());
  }
  else
  {
    bool wifiConnected = wifiClient.connect(networkConfig.ssid, networkConfig.password);
    if (!wifiConnected)
    {
      LOG_ERROR("Failed to connect to the WiFi.");
      // Not connected to wifi, starting the AP mode
      LOG_INFO("Configuring access point (AP)...");
      wifiAP.startAccessPoint(AP_SSID, AP_PASSWORD);
      LOG_INFO("AP IP address:", wifiAP.getIP().toString());
    }
    else
    {
      LOG_INFO("WiFi IP address:", wifiClient.getIP().toString());
    }
  }

  // SERVER setup
  // HTML
  server.serveStatic("/", LittleFS, "/");
  server.on("/", HTTP_GET, homePage);
  // server.onNotFound(not_found_page);

  // API REST
  server.on("/api/v1/wifi/networks", HTTP_GET, fetchWifiNetworks);
  AsyncCallbackJsonWebHandler* updateWifiConfig
      = new AsyncCallbackJsonWebHandler("/api/v1/wifi/config", updateWifiConfiguration);
  server.addHandler(updateWifiConfig);

  server.on("/api/v1/remotes/fetch", HTTP_GET, fetchAllRemotes);
  AsyncCallbackJsonWebHandler* newRemoteHandler
      = new AsyncCallbackJsonWebHandler("/api/v1/remotes/create", createRemote);
  server.addHandler(newRemoteHandler);
  AsyncCallbackJsonWebHandler* deleteRemoteHandler
      = new AsyncCallbackJsonWebHandler("/api/v1/remotes/delete", deleteRemote);
  server.addHandler(deleteRemoteHandler);
  AsyncCallbackJsonWebHandler* updateRemoteHandler
      = new AsyncCallbackJsonWebHandler("/api/v1/remotes/update", updateRemote);
  server.addHandler(updateRemoteHandler);
  AsyncCallbackJsonWebHandler* actionRemoteHandler
      = new AsyncCallbackJsonWebHandler("/api/v1/remotes/action", actionRemote);
  server.addHandler(actionRemoteHandler);

  // Start the server
  server.begin();

  // TEST
  //  Network networks[MAX_NETWORK_SCAN];
  //   wifiClient.getNetworks(networks);
  //   for (int i = 0; i < sizeof(networks) / sizeof(Network); i++) {
  //     LOG_INFO(networks[i].SSID);
  //     LOG_INFO(networks[i].RSSI);
  //   }
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
  // Remote remote = database.getRemote(REMOTE_BASE_ADDRESS + 10);
  // remote.rolling_code += 1;
  // database.updateRemote(remote);
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

  // LOG_INFO("Print all remotes");
  // Remote remotes[MAX_REMOTES];
  // database.getAllRemotes(remotes);
  // for (int i = 0; i < sizeof(remotes) / sizeof(Remote); i++)
  // {
  //   LOG_INFO(remotes[i].id);
  //   LOG_INFO(remotes[i].rolling_code);
  //   LOG_INFO(remotes[i].name);
  // }
};

void loop() {};