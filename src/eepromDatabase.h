#pragma once

#include <Vector.h>

#include "abstracts/database.h"
#include "dto/networks.h"
#include "dto/remote.h"

class EEPROMDatabase : public Database
{
  public:
  void init();

  NetworkConfiguration getNetworkConfiguration();
  bool setNetworkConfiguration(const NetworkConfiguration& networkConfig);
  void resetNetworkConfiguration();

  // CRUD
  Remote createRemote(const char* name);
  void getAllRemotes(Remote remotes[]);
  Remote getRemote(const unsigned long& id);
  bool updateRemote(const Remote& remote);
  bool deleteRemote(const unsigned long& id);

  private:
  int m_networkConfigAddressStart = 0;
  int m_remotesAddressStart = sizeof(NetworkConfiguration);

  bool stringIsAscii(const char* data);
  int getRemoteIndex(const unsigned long& id);
};