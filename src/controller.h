#pragma once

#include "dto/result.h"
#include "abstracts/database.h"
#include "abstracts/serializer.h"
#include "abstracts/transmitter.h"
#include "abstracts/networkClient.h"

class Controller
{
  public:
  Controller(Database* database, NetworkClient* networkClient, Serializer* serializer,
      Transmitter* transmitter);

  Result fetchRemote(const unsigned long id);
  Result fetchAllRemotes();
  Result createRemote(const char* name);
  Result deleteRemote(const unsigned long id);
  Result updateRemote(const unsigned long id, const char* name, const unsigned int rollingCode);
  Result operateRemote(const unsigned long id, const char* action);

  Result updateNetworkConfiguration(const char* ssid, const char* password);

  private:
  Database* m_database;
  NetworkClient* m_networkClient;
  Serializer* m_serializer;
  Transmitter* m_transmitter;
};