#include <Arduino.h>
#include <ArduinoJson.h>

#include "dto/remote.h"
#include "dto/networks.h"

#include "jsonSerializer.h"

String JSONSerializer::serializeRemote(const Remote& remote)
{
  JsonDocument doc;
  JsonObject object = doc.to<JsonObject>();

  this->serializeRemote(object, remote);

  String output;
  serializeJson(doc, output);
  return output;
};

String JSONSerializer::serializeRemotes(const Remote remotes[], int size)
{
  JsonDocument doc;
  JsonArray array = doc.to<JsonArray>();

  for (int i = 0; i < size; i++)
  {
    if (remotes[i].id == 0)
    {
      // Empty remote
      continue;
    }
    JsonObject object = array.createNestedObject();
    this->serializeRemote(object, remotes[i]);
  }

  String output;
  serializeJson(doc, output);
  return output;
};

String JSONSerializer::serializeNetworkConfig(const NetworkConfiguration& networkConfig)
{
  JsonDocument doc;
  JsonObject object = doc.to<JsonObject>();

  object["ssid"] = networkConfig.ssid;
  object["password"] = networkConfig.password;

  String output;
  serializeJson(doc, output);
  return output;
};

String JSONSerializer::serializeNetworks(const Network networks[], int size)
{
  String output;
  return output;
};

// PRIVATE

void JSONSerializer::serializeRemote(JsonObject object, const Remote& remote)
{
  object["id"] = remote.id;
  object["rolling_code"] = remote.rollingCode;
  object["name"] = remote.name;
};