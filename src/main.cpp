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
// WEBSERVER CALLBACKS HTML
// ============================================================================
void homePage(AsyncWebServerRequest* request)
{
  LOG_DEBUG("HTML home page reached.");
  request->send(LittleFS, "/index.html", String());
};

// ============================================================================
// WEBSERVER CALLBACKS RESTAPI
// ============================================================================
void handleCoreRestart(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to restart module reached.");
  request->send(200, "application/json", "{\"message\":\"Restarting...\"}");
  ESP.restart();
}

void handleFetchWifiNetworks(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to fetch Wifi Networks reached.");
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
}

void handleFetchWifiConfiguration(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to fetch Network Configuration reached.");
  Result result = controller.fetchNetworkConfiguration();
  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.error + "\"}");
    return;
  }
  request->send(200, "application/json", result.data);
}

void handleUpdateWifiConfiguration(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to update the Wifi Wonfiguration reached.");

  String ssid;
  String password;
  if (request->hasParam("ssid", true))
  {
    AsyncWebParameter* p = request->getParam("ssid", true);
    ssid = p->value();
  }
  if (request->hasParam("password", true))
  {
    AsyncWebParameter* p = request->getParam("password", true);
    password = p->value();
  }

  Result result = controller.updateNetworkConfiguration(ssid.c_str(), password.c_str());
  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.error + "\"}");
    return;
  }
  request->send(201, "application/json", result.data);
}

void handleFetchAllRemotes(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to fetch all remotes reached.");
  Result result = controller.fetchAllRemotes();
  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.error + "\"}");
    return;
  }
  request->send(200, "application/json", result.data);
}

void handleFetchRemote(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to fetch a remote reached.");
  unsigned long remoteId = request->pathArg(0).toInt();

  Result result = controller.fetchRemote(remoteId);
  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.error + "\"}");
    return;
  }
  request->send(200, "application/json", result.data);
}

void handleCreateRemote(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to create a remote reached.");

  String name;
  if (request->hasParam("name", true))
  {
    AsyncWebParameter* p = request->getParam("name", true);
    name = p->value();
  }

  Result result = controller.createRemote(name.c_str());
  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.error + "\"}");
    return;
  }
  request->send(201, "application/json", result.data);
}

void handleUpdateRemote(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to update a remote reached.");
  unsigned long remoteId = request->pathArg(0).toInt();

  String name;
  unsigned int rollingCode = 0;

  if (request->hasParam("name", true))
  {
    AsyncWebParameter* p = request->getParam("name", true);
    name = p->value();
  }

  if (request->hasParam("rolling_code", true))
  {
    AsyncWebParameter* p = request->getParam("rolling_code", true);
    rollingCode = int(p->value().toInt());
  }

  Result result = controller.updateRemote(remoteId, name.c_str(), rollingCode);
  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.error + "\"}");
    return;
  }
  request->send(200, "application/json", result.data);
}

void handleDeleteRemote(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to delete a remote reached.");
  unsigned long remoteId = request->pathArg(0).toInt();

  Result result = controller.deleteRemote(remoteId);
  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.error + "\"}");
    return;
  }
  request->send(200, "application/json", "{\"message\":\"The remote has been deleted.\"}");
}

void handleActionRemote(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to operate an action on a remote reached.");
  unsigned long remoteId = request->pathArg(0).toInt();

  String action;
  if (request->hasParam("action", true))
  {
    AsyncWebParameter* p = request->getParam("action", true);
    action = p->value();
  }

  Result result = controller.operateRemote(remoteId, action.c_str());
  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.error + "\"}");
    return;
  }
  request->send(200, "application/json", "{\"message\":\"" + result.data + "\"}");
}

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
  server.on("/api/v1/core/restart", HTTP_POST, handleCoreRestart);
  server.on("/api/v1/wifi/networks", HTTP_GET, handleFetchWifiNetworks);
  server.on("/api/v1/wifi/config", HTTP_GET, handleFetchWifiConfiguration);
  server.on("/api/v1/wifi/config", HTTP_POST, handleUpdateWifiConfiguration);
  server.on("^\\/api/v1/remotes$", HTTP_GET, handleFetchAllRemotes);
  server.on("^\\/api/v1/remotes$", HTTP_POST, handleCreateRemote);
  server.on("^\\/api/v1/remotes\\/([0-9]+)$", HTTP_GET, handleFetchRemote);
  server.on("^\\/api/v1/remotes\\/([0-9]+)$", HTTP_PATCH, handleUpdateRemote);
  server.on("^\\/api/v1/remotes\\/([0-9]+)$", HTTP_DELETE, handleDeleteRemote);
  server.on("^\\/api/v1/remotes\\/([0-9]+)\\/action$", HTTP_POST, handleActionRemote);

  // Start the server
  server.begin();
}

void loop()
{
  // put your main code here, to run repeatedly:
}