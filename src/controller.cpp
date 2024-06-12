#include <Arduino.h>
#include <DebugLog.h>

#include "controller.h"

#include "../default_config.h"
#include "dto/remote.h"
#include "dto/result.h"
#include "dto/networks.h"
#include "abstracts/database.h"
#include "abstracts/serializer.h"
#include "abstracts/networkClient.h"

Controller::Controller(Database* database, NetworkClient* networkClient, Serializer* serializer)
    : m_database(database)
    , m_networkClient(networkClient)
    , m_serializer(serializer)
{
}

Result Controller::fetchRemote(const unsigned long id)
{
  LOG_DEBUG("Fetching Remote...");
  Result result;
  if (id == 0)
  {
    LOG_ERROR("The remote id is not specified.");
    result.error = "The remote id is not specified.";
    return result;
  }

  Remote remote = this->m_database->getRemote(id);

  if (remote.id == 0)
  {
    LOG_ERROR("This remote doesn't exists.");
    result.error = "This remote doesn't exists.";
    return result;
  }

  result.isSuccess = true;
  String serialized = this->m_serializer->serializeRemote(remote);
  result.data = serialized;
  LOG_DEBUG("Remote fetched.");
  return result;
}

Result Controller::fetchAllRemotes()
{
  LOG_DEBUG("Fetching all remotes...");
  Remote remotes[MAX_REMOTES];
  this->m_database->getAllRemotes(remotes);

  String serialized = this->m_serializer->serializeRemotes(remotes, MAX_REMOTES);

  Result result = { serialized, "", true };

  LOG_DEBUG("All Remotes fetched.");
  return result;
}

Result Controller::createRemote(const char* name)
{
  LOG_DEBUG("Creating a new Remote...");
  Result result;
  if (name == nullptr)
  {
    LOG_ERROR("The name of the remote should be specified.");
    result.error = "The name of the remote should be specified.";
    return result;
  }

  if (strlen(name) == 0)
  {
    LOG_ERROR("The name of the remote cannot be empty.");
    result.error = "The name of the remote cannot be empty.";
    return result;
  }

  if (strlen(name) > MAX_REMOTE_NAME_LENGTH)
  {
    LOG_ERROR("The name is too long.");
    String error = "The name is too long. It can contain only " + String(MAX_REMOTE_NAME_LENGTH - 1)
        + " chars.";
    result.error = error;
    return result;
  }

  Remote remote = this->m_database->createRemote(name);

  if (remote.id == 0)
  {
    LOG_ERROR(
        "The created remote is an empty remote. No space left on the device for a new remote.");
    result.error = "No space left on the device for a new remote.";
    return result;
  }

  result.isSuccess = true;
  String serialized = this->m_serializer->serializeRemote(remote);
  result.data = serialized;
  LOG_DEBUG("Remote created.");
  return result;
}

Result Controller::deleteRemote(const unsigned long id)
{
  LOG_DEBUG("Deleting Remote...");
  Result result;
  if (id == 0)
  {
    LOG_ERROR("The remote id is not specified.");
    result.error = "The remote id is not specified.";
    return result;
  }

  bool isDeleted = this->m_database->deleteRemote(id);

  if (!isDeleted)
  {
    LOG_ERROR("The given remote doesn't exist in the database.");
    result.error = "The given remote doesn't exist in the database.";
    return result;
  }

  result.isSuccess = true;
  LOG_DEBUG("Remote deleted.");
  return result;
}

Result Controller::updateRemote(
    const unsigned long id, const char* name, const unsigned int rollingCode)
{
  LOG_DEBUG("Updating Remote...");
  Result result;
  if (id == 0)
  {
    LOG_ERROR("The remote id should be specified.");
    result.error = "The remote id should be specified.";
    return result;
  }

  Remote remote = this->m_database->getRemote(id);

  if (remote.id == 0)
  {
    LOG_ERROR("The remote doesn't exist. It cannot be updated.");
    result.error = "The remote doesn't exist. It cannot be updated.";
    return result;
  }

  if (name != nullptr)
  {
    if (strlen(name) > MAX_REMOTE_NAME_LENGTH)
    {
      LOG_ERROR("The name is too long.");
      String error = "The name is too long. It can contain only "
          + String(MAX_REMOTE_NAME_LENGTH - 1) + " chars.";
      result.error = error;
      return result;
    }
    if (strlen(name) != 0)
    {
      LOG_DEBUG("The name of the remote will be updated.");
      strcpy(remote.name, name);
    }
  }
  else
  {
    LOG_DEBUG("The name of the remote is not specified. It will not change.");
  }

  if (rollingCode != 0)
  {
    LOG_DEBUG("The rolling code of the remote will be updated.");
    // TODO. We want this ?
  }
  else
  {
    LOG_DEBUG("The rolling code of the remote is not specified. It will not change.");
  }

  bool isUpdated = this->m_database->updateRemote(remote);

  if (!isUpdated)
  {
    LOG_ERROR("Failed to update the remote.");
    result.error = "Something went wrong while updating the remote.";
    return result;
  }

  result.isSuccess = true;
  String serialized = this->m_serializer->serializeRemote(remote);
  result.data = serialized;
  LOG_DEBUG("Remote updated.");
  return result;
}

Result Controller::operateRemote(const unsigned long id, const char* action)
{
  LOG_DEBUG("Operating a command with the Remote...");
  Result result;
  if (id == 0)
  {
    LOG_ERROR("The remote id should be specified.");
    result.error = "The remote id should be specified.";
    return result;
  }

  if (action == nullptr)
  {
    LOG_ERROR("The action should be specified. Allowed actions: up, down, stop, pair, reset.");
    result.error = "The action should be specified. Allowed actions: up, down, stop, pair, reset.";
    return result;
  }

  Remote remote = this->m_database->getRemote(id);

  if (remote.id == 0)
  {
    LOG_ERROR("The remote doesn't exist. It cannot be operate.");
    result.error = "The remote doesn't exist. It cannot be operate.";
    return result;
  }

  if (strcmp(action, "up") == 0)
  {
    LOG_DEBUG("Operate 'UP'.");
  }
  else if (strcmp(action, "stop") == 0)
  {
    LOG_DEBUG("Operate 'STOP'.");
  }
  else if (strcmp(action, "down") == 0)
  {
    LOG_DEBUG("Operate 'DOWN'.");
  }
  else if (strcmp(action, "pair") == 0)
  {
    LOG_DEBUG("Operate 'PAIR'.");
  }
  else if (strcmp(action, "reset") == 0)
  {
    LOG_DEBUG("Operate 'RESET'.");
  }
  else
  {
    LOG_WARN("The action is not valid.");
    result.error = "The action is not valid. Allowed actions: up, down, stop, pair, reset.";
    return result;
  }

  result.isSuccess = true;
  result.data = "Command sent.";
  LOG_DEBUG("Command sent with the remote.");
  return result;
}

Result Controller::updateNetworkConfiguration(const char* ssid, const char* password)
{
  LOG_DEBUG("Updating Network Configuration...");
  Result result;
  if (ssid == nullptr)
  {
    LOG_ERROR("The ssid should be specified.");
    result.error = "The ssid should be specified.";
    return result;
  }

  if (strlen(ssid) == 0)
  {
    LOG_ERROR("The ssid cannot be empty.");
    result.error = "The ssid cannot be empty.";
    return result;
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

  bool isUpdated = this->m_database->setNetworkConfiguration(networkConfig);

  if (!isUpdated)
  {
    LOG_ERROR("Something went wrong while updating the Network Configuration");
    result.error = "Something went wrong while updating the Network Configuration";
    return result;
  }

  result.isSuccess = true;
  String serialized = this->m_serializer->serializeNetworkConfig(networkConfig);
  result.data = serialized;
  LOG_DEBUG("Network Configuration updated.");
  return result;
}