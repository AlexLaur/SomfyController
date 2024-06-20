/**
 * @file eepromDatabase.cpp
 * @author Laurette Alexandre
 * @brief Implementation of EEPROM database storage system.
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
#include <EEPROM.h>

#include <DebugLog.h>

#include <config.h>
#include <remote.h>
#include <networks.h>
#include <eepromDatabase.h>

/**
 * @brief Initialise the Database in the EEPROM of the ESP
 *
 */
void EEPROMDatabase::init()
{
  this->migrate();

  size_t totalSize = sizeof(NetworkConfiguration) + sizeof(Remote) * MAX_REMOTES;
  LOG_DEBUG("Allocating EEPROM space: ", totalSize);
  EEPROM.begin(totalSize);

  // Todo wipe to emptyRemote all corupted remotes
  LOG_DEBUG("Reseting all corrupted remotes...");
  Remote remoteRead;
  Remote emptyRemote = { 0, 0, "" };
  int count = 0;
  for (int index = 0; index < MAX_REMOTES; ++index)
  {
    EEPROM.get(this->m_remotesAddressStart + index * sizeof(Remote), remoteRead);

    // Non ASCII chars in the name = Invalid
    if (!stringIsAscii(remoteRead.name))
    {
      LOG_WARN("Invalid name found on remote:", remoteRead.id);
      LOG_WARN("This remote will be removed.");
      EEPROM.put(this->m_remotesAddressStart + index * sizeof(Remote), emptyRemote);
      count++;
      continue;
    }

    // An ID < REMOTE_BASE_ADDRESS OR ID > (REMOTE_BASE_ADDRESS + MAX_REMOTES) = Invalid
    if (remoteRead.id < REMOTE_BASE_ADDRESS || remoteRead.id > (REMOTE_BASE_ADDRESS + MAX_REMOTES))
    {
      if (remoteRead.id == 0)
      {
        // It is an empty remote.
        continue;
      }
      EEPROM.put(this->m_remotesAddressStart + index * sizeof(Remote), emptyRemote);
      count++;
      continue;
    }
  }
  EEPROM.commit();
  LOG_DEBUG("Corrupted Remotes detected and reseted: ", count);
}

/**
 * @brief Get the network configuration.
 * If the configuration is corrupted. An empty config is returned (empty SSID and password).
 *
 * @return NetworkConfiguration
 */
NetworkConfiguration EEPROMDatabase::getNetworkConfiguration()
{
  NetworkConfiguration networkConfig;
  EEPROM.get(this->m_networkConfigAddressStart, networkConfig);
  if (!this->stringIsAscii(networkConfig.ssid) || !this->stringIsAscii(networkConfig.password))
  {
    LOG_ERROR("The networkConfig seems to be corrupted. An empty config will be returned.");
    LOG_DEBUG("SSID:", networkConfig.ssid);
    LOG_DEBUG("Password: ", networkConfig.password);

    strcpy(networkConfig.ssid, "");
    strcpy(networkConfig.password, "");
  }
  return networkConfig;
}

/**
 * @brief Save a new network configuration in the EEPROM
 *
 * @param networkConfig
 */
bool EEPROMDatabase::setNetworkConfiguration(const NetworkConfiguration& networkConfig)
{
  LOG_DEBUG("Saving new network configuration...");
  EEPROM.put(this->m_networkConfigAddressStart, networkConfig);
  EEPROM.commit();
  LOG_INFO("Network configuration saved.");
  return true;
}

/**
 * @brief Reset the current network configuration
 *
 */
void EEPROMDatabase::resetNetworkConfiguration()
{
  LOG_DEBUG("Reseting network configuration...");
  NetworkConfiguration networkConfig = { "", "" };
  this->setNetworkConfiguration(networkConfig);
  LOG_INFO("Network configuration reseted.");
}

/**
 * @brief Get all remotes in the database
 *
 * @param remotes Array for remotes. Should be an array with a size of MAX_REMOTES, defined in the
 * config file.
 */
void EEPROMDatabase::getAllRemotes(Remote remotes[])
{
  LOG_DEBUG("Getting all remotes...");
  Remote remoteRead;
  for (int i = 0; i < MAX_REMOTES; ++i)
  {
    EEPROM.get(this->m_remotesAddressStart + i * sizeof(Remote), remoteRead);
    remotes[i].id = remoteRead.id;
    remotes[i].rollingCode = remoteRead.rollingCode;
    strcpy(remotes[i].name, remoteRead.name);
  }
}

/**
 * @brief Get a specific remote
 *
 * @param id The id of the remote
 * @return Remote The remote in the database or an empty remote if the given id is not found.
 */
Remote EEPROMDatabase::getRemote(const unsigned long& id)
{
  LOG_DEBUG("Looking for the remote with the ID:", id);
  int index = this->getRemoteIndex(id);
  if (index < 0)
  {
    LOG_WARN("No Remote found.");
    Remote emptyRemote = { 0, 0, "" };
    return emptyRemote;
  }

  LOG_DEBUG("Remote found.");
  Remote remoteRead;
  EEPROM.get(this->m_remotesAddressStart + index * sizeof(Remote), remoteRead);

  return remoteRead;
}

/**
 * @brief Remove a remote from the database
 *
 * @param id The id of the remote to delete.
 * @return true if the remote has been deleted
 * @return false otherwise
 */
bool EEPROMDatabase::deleteRemote(const unsigned long& id)
{
  LOG_DEBUG("Removing remote with the ID:", id);
  int index = this->getRemoteIndex(id);
  if (index < 0)
  {
    LOG_WARN("No Remote found for the given id. Nothing to remove.");
    return false;
  }
  Remote emptyRemote = { 0, 0, "" };
  EEPROM.put(this->m_remotesAddressStart + index * sizeof(Remote), emptyRemote);
  EEPROM.commit();
  LOG_DEBUG("The remote has been deleted.");
  return true;
}

/**
 * @brief Add a new remote in the database.
 *
 * @param name The name of the remote.
 * @return Remote The created remote.
 */
Remote EEPROMDatabase::createRemote(const char* name)
{
  LOG_DEBUG("Adding a new remote...");
  // A remote with this ID is an empty remote. We return the first found.
  int index = this->getRemoteIndex(0);
  Remote emptyRemote = { 0, 0, "" };
  if (index < 0)
  {
    LOG_ERROR("No space left. Cannot add a new remote.");
    return emptyRemote;
  }
  emptyRemote.id = REMOTE_BASE_ADDRESS + index;
  emptyRemote.rollingCode = 0;
  strcpy(emptyRemote.name, name);

  EEPROM.put(this->m_remotesAddressStart + index * sizeof(Remote), emptyRemote);
  EEPROM.commit();

  LOG_DEBUG("A new remote has been added.");
  return emptyRemote;
}

/**
 * @brief update the remote in the database
 *
 * @param remote The remote to update
 * @return true if the update was done
 * @return false otherwise
 */
bool EEPROMDatabase::updateRemote(const Remote& remote)
{
  LOG_DEBUG("Updating remote ID:", remote.id);
  int index = this->getRemoteIndex(remote.id);
  if (index < 0)
  {
    LOG_WARN("The remote doesn't exist in the table. It cannot be updated.");
    return false;
  }
  EEPROM.put(this->m_remotesAddressStart + index * sizeof(Remote), remote);
  EEPROM.commit();
  LOG_DEBUG("The remote has been updated.");
  return true;
}

// PRIVATE
/**
 * @brief Check if a string (char*) contains non-ascii chars.
 *
 * @param data The data to analyse
 * @return true if the string is ASCII
 * @return false otherwise
 */
bool EEPROMDatabase::stringIsAscii(const char* data)
{
  int it = 0;
  while (data[it] != '\0')
  {
    if (!isAscii(data[it]))
    {
      return false;
    }
    ++it;
  }
  return true;
}

int EEPROMDatabase::getRemoteIndex(const unsigned long& id)
{
  Remote remoteRead;
  for (int i = 0; i < MAX_REMOTES; ++i)
  {
    EEPROM.get(this->m_remotesAddressStart + i * sizeof(Remote), remoteRead);
    if (remoteRead.id == id)
    {
      return i;
    }
  }
  return -1;
}

/**
 * @brief Apply migration on the database.
 * Usefull for future versions releases if some parts change in the database.
 *
 * @return true if the migration succeded
 * @return false otherwise
 */
bool EEPROMDatabase::migrate()
{
  LOG_INFO("Apply database migrations...");
  return true;
}