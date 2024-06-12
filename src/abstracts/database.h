#pragma once

#include "../wifiClient.h"
#include "../dto/networks.h"
#include "../dto/remote.h"

class Database
{
  public:
  virtual void init() = 0;

  virtual NetworkConfiguration getNetworkConfiguration() = 0;
  virtual bool setNetworkConfiguration(const NetworkConfiguration& networkConfig) = 0;
  virtual void resetNetworkConfiguration() = 0;

  // CRUD methods for remote
  virtual Remote createRemote(const char* name) = 0;
  virtual void getAllRemotes(Remote remotes[]) = 0;
  virtual Remote getRemote(const unsigned long& id) = 0;
  virtual bool updateRemote(const Remote& remote) = 0;
  virtual bool deleteRemote(const unsigned long& id) = 0;
};