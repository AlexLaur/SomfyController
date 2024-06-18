/**
 * @file SomfyController.ino
 * @author Laurette Alexandre
 * @brief Control SOMFY blinds.
 * @version 2.0.0
 * @date 2024-06-06
 *
 * @copyright (c) 2024 Laurette Alexandre
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#define DEBUGLOG_DEFAULT_LOG_LEVEL_DEBUG

#include <Arduino.h>
#include <DebugLog.h>
#include <LittleFS.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <config.h>
#include <remote.h>
#include <networks.h>
#include <controller.h>
#include <wifiClient.h>
#include <wifiAccessPoint.h>
#include <RTSTransmitter.h>
#include <eepromDatabase.h>
#include <jsonSerializer.h>

EEPROMDatabase database;
WifiClient wifiClient;
WifiAccessPoint wifiAP;
JSONSerializer serializer;
RTSTransmitter transmitter;

AsyncWebServer server(SERVER_PORT);
Network networks[MAX_NETWORK_SCAN];
Controller controller(&database, &wifiClient, &serializer, &transmitter);

// ============================================================================
// WEBSERVER CALLBACKS
// ============================================================================
void homePage(AsyncWebServerRequest* request)
{
  LOG_DEBUG("HTML home page reached.");
  request->send(LittleFS, "/index.html", String());
};

void fetchWifiNetworks(AsyncWebServerRequest* request)
{
  LOG_DEBUG("Endpoint to fetch Wifi Networks reached.");
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
  LOG_DEBUG("Endpoint to fetch all remotes reached.");
  Result result = controller.fetchAllRemotes();
  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.error + "\"}");
    return;
  }
  request->send(200, "application/json", result.data);
};

void createRemote(AsyncWebServerRequest* request, JsonVariant& json)
{
  LOG_DEBUG("Endpoint to create a remote reached.");
  const JsonObject& payload = json.as<JsonObject>();
  const char* name = payload["name"];

  Result result = controller.createRemote(name);

  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.error + "\"}");
    return;
  }
  request->send(201, "application/json", result.data);
};

void deleteRemote(AsyncWebServerRequest* request, JsonVariant& json)
{
  LOG_DEBUG("Endpoint to delete a remote reached.");
  const JsonObject& payload = json.as<JsonObject>();
  const unsigned long id = payload["remote_id"];

  Result result = controller.deleteRemote(id);

  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.error + "\"}");
    return;
  }
  request->send(200, "application/json", "{\"message\":\"The remote has been deleted.\"}");
}

void updateWifiConfiguration(AsyncWebServerRequest* request, JsonVariant& json)
{
  LOG_DEBUG("Endpoint to update the Wifi Wonfiguration reached.");
  const JsonObject& data = json.as<JsonObject>();
  const char* ssid = data["ssid"];
  const char* password = data["password"];

  Result result = controller.updateNetworkConfiguration(ssid, password);
  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.error + "\"}");
    return;
  }
  request->send(201, "application/json", result.data);
};

void updateRemote(AsyncWebServerRequest* request, JsonVariant& json)
{
  LOG_DEBUG("Endpoint to update a remote reached.");
  const JsonObject& payload = json.as<JsonObject>();
  const unsigned long id = payload["remote_id"];
  const JsonObject data = payload["data"];
  const char* name = data["name"];
  const unsigned int rollingCode = data["rolling_code"];

  Result result = controller.updateRemote(id, name, rollingCode);
  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.error + "\"}");
    return;
  }
  request->send(200, "application/json", result.data);
};

void actionRemote(AsyncWebServerRequest* request, JsonVariant& json)
{
  LOG_DEBUG("Endpoint to operate an action on a remote reached.");
  const JsonObject& payload = json.as<JsonObject>();

  const unsigned long id = payload["remote_id"];
  const char* action = payload["action"];

  Result result = controller.operateRemote(id, action);
  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.error + "\"}");
    return;
  }
  request->send(200, "application/json", "{\"message\":\"" + result.data + "\"}");
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
  delay(5000);

  // Open the output for 433.42MHz and 433.92MHz transmitter
  LOG_INFO("Initializing pin for transmitter...");
  transmitter.init();

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
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
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
}

void loop()
{
  // put your main code here, to run repeatedly:
}