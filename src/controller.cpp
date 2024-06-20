/**
 * @file controller.cpp
 * @author Laurette Alexandre
 * @brief Implementation of the controller of the application.
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

#include <controller.h>

#include <config.h>
#include <remote.h>
#include <result.h>
#include <networks.h>
#include <databaseAbs.h>
#include <serializerAbs.h>
#include <transmitterAbs.h>
#include <networkClientAbs.h>

Controller::Controller(DatabaseAbstract* database, NetworkClientAbstract* networkClient, SerializerAbstract* serializer,
    TransmitterAbstract* transmitter)
    : m_database(database)
    , m_networkClient(networkClient)
    , m_serializer(serializer)
    , m_transmitter(transmitter)
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
    LOG_WARN("The rolling code cannot be updated yet.");
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
  LOG_INFO("Operating a command with the Remote", id);
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

  if (strlen(action) == 0){
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
    LOG_INFO("Operate 'UP'.");
    this->m_transmitter->sendUpCmd(remote.id, remote.rollingCode);
    result.data = "Command UP sent.";
  }
  else if (strcmp(action, "stop") == 0)
  {
    LOG_INFO("Operate 'STOP'.");
    this->m_transmitter->sendStopCmd(remote.id, remote.rollingCode);
    result.data = "Command STOP sent.";
  }
  else if (strcmp(action, "down") == 0)
  {
    LOG_INFO("Operate 'DOWN'.");
    this->m_transmitter->sendDownCmd(remote.id, remote.rollingCode);
    result.data = "Command DOWN sent.";
  }
  else if (strcmp(action, "pair") == 0)
  {
    LOG_INFO("Operate 'PAIR'.");
    this->m_transmitter->sendProgCmd(remote.id, remote.rollingCode);
    result.data = "Command PAIR sent.";
  }
  else if (strcmp(action, "reset") == 0)
  {
    LOG_INFO("Operate 'RESET'.");
    remote.rollingCode = 0;
    this->m_database->updateRemote(remote);
    result.isSuccess = true;
    result.data = "Rolling code reseted.";
    return result;
  }
  else
  {
    LOG_WARN("The action is not valid.");
    result.error = "The action is not valid. Allowed actions: up, down, stop, pair, reset.";
    return result;
  }

  result.isSuccess = true;
  remote.rollingCode += 1; // increment rollingCode
  this->m_database->updateRemote(remote);
  LOG_INFO("Command sent through the remote", remote.id);
  return result;
}

Result Controller::fetchNetworkConfiguration(){
  LOG_DEBUG("Fetching Network Configuration...");
  Result result;

  NetworkConfiguration networkConfig = this->m_database->getNetworkConfiguration();

  result.isSuccess = true;
  String serialized = this->m_serializer->serializeNetworkConfig(networkConfig);
  result.data = serialized;
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