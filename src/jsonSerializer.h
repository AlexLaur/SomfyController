#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "dto/remote.h"
#include "dto/networks.h"
#include "abstracts/serializer.h"

class JSONSerializer : public Serializer
{
  public:
  String serializeRemote(const Remote& remote);
  String serializeRemotes(const Remote remotes[], int size);
  String serializeNetworkConfig(const NetworkConfiguration& networkConfig);
  String serializeNetworks(const Network networks[], int size);

  private:
    void serializeRemote(JsonObject object, const Remote& remote);
};