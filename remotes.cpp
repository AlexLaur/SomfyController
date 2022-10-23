/**
 * @file remotes.cpp
 * @author Laurette Alexandre
 * @brief Remotes implementations
 * @version 0.1
 * @date 2022-10-22
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <EEPROM.h>

#include <Vector.h>
#include <Logger.h>

#include "remotes.h"

const unsigned long RemotesManager::REMOTE_BASE_ADDRESS = 0x100000;

/**
 * @brief Construct a new Remotes Container:: Remotes Container object
 *
 * @param remotes
 */
RemotesManager::RemotesManager(Vector<Remote> remotes)
{
    Logger::verbose("RemotesManager::RemotesManager", "Initialisation of RemotesManager.");
    EEPROM.begin(512);
    this->remotes = remotes;
};

/**
 * @brief Destroy the Remotes Container:: Remotes Container object
 *
 */
RemotesManager::~RemotesManager()
{
    Logger::verbose("RemotesManager::~RemotesManager", "RemotesManager destroyed.");
    EEPROM.end();
}

/**
 * @brief Load all remotes from the config and link this remotes with remotes
 * saved into the memory. If remotes into the memory doesn't exists or seems
 * to be corrupted, the remote will be re writted.
 *
 * @param remote_names
 * @param nb_remotes
 */
void RemotesManager::load_remotes(const char* remote_names[], int nb_remotes)
{
    Logger::verbose("RemotesManager::load_remotes()", "Loading all remotes...");
    for (int i = 0; i < nb_remotes; i++)
    {
        unsigned long id = this->REMOTE_BASE_ADDRESS + i;
        unsigned int rolling_code = 0;
        char* name = (char*)remote_names[i];
        bool enabled = false;
        unsigned short eeprom_address = i * sizeof(RemoteState);
        this->remotes.push_back({ id, rolling_code, name, enabled, eeprom_address });
    }
    this->update_remotes_from_memory();
    Logger::notice("RemotesManager::load_remotes()", "Remotes loaded.");
};

/**
 * @brief PrettyPrint for all remotes through the Serial.
 *
 */
void RemotesManager::print_remotes()
{
    for (Remote remote : this->remotes)
    {
        Serial.print("Name: ");
        Serial.println(remote.name);
        Serial.print("Id: ");
        Serial.println(remote.id, HEX);
        Serial.print("Rolling code: ");
        Serial.println(remote.rolling_code);
        Serial.print("Enabled : ");
        Serial.println(remote.enabled);
        Serial.println("----------------------");
    }
}

/**
 * @brief Get all configured remotes
 *
 * @return Vector<Remote>
 */
Vector<Remote> RemotesManager::get_remotes() { return this->remotes; };

/**
 * @brief Update the remote inside the Vector and inside the memory if the
 * remote.id exists in the container.
 *
 * @param remote
 */
void RemotesManager::update_remote(Remote remote) { this->update(remote); };

/**
 * @brief Reset the rolling code of the given remote. It will update this into
 * the saved remote.
 *
 * @param remote
 */
void RemotesManager::reset_rolling_code(Remote remote)
{
    Logger::verbose("RemotesManager::reset_rolling_code()", "Reseting remote rolling code...");
    remote.rolling_code = 0;
    this->update(remote);
    Logger::notice("RemotesManager::reset_rolling_code()", "Reset rolling code done.");
};

/**
 * @brief Reset the rolling code of the given remote (By its ID). It will
 * update this into the saved remote.
 *
 * @param remote_id
 */
void RemotesManager::reset_rolling_code(unsigned long remote_id)
{
    Logger::verbose("RemotesManager::reset_rolling_code()", "Reseting remote rolling code...");
    for (Remote remote : this->remotes)
    {
        if (remote.id != remote_id)
        {
            continue;
        }
        remote.rolling_code = 0;
        this->update(remote);
    }
    Logger::notice("RemotesManager::reset_rolling_code()", "Reset rolling code done.");
};

/**
 * @brief Toggle the state of the remote. If the remote is enabled, it will
 * be disabled. If the toggle is disabled, it will be enabled.
 *
 * @param remote
 */
void RemotesManager::toggle_remote_enable(Remote remote)
{
    Logger::verbose("RemotesManager::toggle_remote_enable()", "Toggleing remote enabled field.");
    for (Remote current_remote : this->remotes)
    {
        if (remote.id != current_remote.id)
        {
            continue;
        }
        current_remote.enabled = !current_remote.enabled;
        this->update(current_remote);
    }
    Logger::notice("RemotesManager::toggle_remote_enable()", "Toggle remote enabled field done.");
};

/**
 * @brief Toggle the state of the remote. If the remote is enabled, it will
 * be disabled. If the toggle is disabled, it will be enabled.
 *
 * @param remote_id
 */
void RemotesManager::toggle_remote_enable(unsigned long remote_id)
{
    Logger::verbose("RemotesManager::toggle_remote_enable()", "Toggleing remote enabled field.");
    for (Remote remote : this->remotes)
    {
        if (remote.id != remote_id)
        {
            continue;
        }
        remote.enabled = !remote.enabled;
        this->update(remote);
    }
    Logger::notice("RemotesManager::toggle_remote_enable()", "Toggle remote enabled field done.");
};

// PRIVATE

void RemotesManager::update(Remote remote)
{
    char s[32]; // used for logger messages
    snprintf_P(s, sizeof(s), PSTR("Updating the remote: %x"), remote.id);
    Logger::notice("RemotesManager::update()", s);

    for (unsigned int i = 0; i < this->remotes.size(); i++)
    {
        if (this->remotes[i].id != remote.id)
        {
            continue;
        }
        this->remotes[i].rolling_code = remote.rolling_code;
        this->remotes[i].enabled = remote.enabled;

        RemoteState remote_state
            = { this->remotes[i].id, this->remotes[i].rolling_code, this->remotes[i].enabled };

        EEPROM.put(this->remotes[i].eeprom_address, remote_state);
        EEPROM.commit();

        Logger::notice("RemotesManager::update()", "Remote updated.");
        return;
    }

    snprintf_P(s, sizeof(s), PSTR("No remote found with id %x."), remote.id);
    Logger::warning("RemotesManager::update()", s);
    Logger::warning("RemotesManager::update()", "Nothing has been updated.");
};

void RemotesManager::update_remotes_from_memory()
{
    char s[64]; // used for logger messages
    Logger::verbose("RemotesManager::update_remotes_from_memory()",
        "Update all remotes with values stored in the memory.");

    for (unsigned int i = 0; i < this->remotes.size(); i++)
    {
        RemoteState saved_remote;
        EEPROM.get(this->remotes[i].eeprom_address, saved_remote);

        if (this->remotes[i].id != saved_remote.id)
        {
            snprintf_P(s, sizeof(s), PSTR("Remote '%s' at '%x' seems to be corrupted."),
                this->remotes[i].name, this->remotes[i].id);
            Logger::warning("RemotesManager::update_remotes_from_memory()", s);
            Logger::warning("RemotesManager::update_remotes_from_memory()",
                "The state of this remote will be re-writted.");

            saved_remote.id = this->remotes[i].id;
            saved_remote.enabled = this->remotes[i].enabled;
            saved_remote.rolling_code = this->remotes[i].rolling_code;

            EEPROM.put(this->remotes[i].eeprom_address, saved_remote);
            EEPROM.commit();

            Logger::notice(
                "RemotesManager::update_remotes_from_memory()", "Remote writted into the memory.");
        }

        snprintf_P(s, sizeof(s), PSTR("Updating remote '%x' with values from the memory..."),
            this->remotes[i].id);
        Logger::verbose("RemotesManager::update_remotes_from_memory()", s);

        // update the current remote with EEPROM values
        this->remotes[i].enabled = saved_remote.enabled;
        this->remotes[i].rolling_code = saved_remote.rolling_code;
    }
};