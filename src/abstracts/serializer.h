#pragma once

#include <Arduino.h>
#include "../dto/remote.h"
#include "../dto/networks.h"

class Serializer
{
  public:
  virtual String serializeRemote(const Remote& remote) = 0;
  virtual String serializeRemotes(const Remote remotes[], int size) = 0;
  virtual String serializeNetworkConfig(const NetworkConfiguration& networkConfig) = 0;
  virtual String serializeNetworks(const Network networks[], int size) = 0;
};