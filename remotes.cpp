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

const unsigned long RemotesContainer::REMOTE_BASE_ADDRESS = 0x100000;

/**
 * @brief Construct a new Remotes Container:: Remotes Container object
 *
 * @param remotes
 */
RemotesContainer::RemotesContainer(Vector<Remote> remotes)
{
    Logger::verbose("RemotesContainer::RemotesContainer",
        "Initialisation of RemotesContainer.");
    EEPROM.begin(512);
    this->remotes = remotes;
};

/**
 * @brief Destroy the Remotes Container:: Remotes Container object
 *
 */
RemotesContainer::~RemotesContainer()
{
    Logger::verbose(
        "RemotesContainer::~RemotesContainer", "RemotesContainer destroyed.");
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
void RemotesContainer::load_remotes(const char* remote_names[], int nb_remotes)
{
    Logger::verbose(
        "RemotesContainer::load_remotes()", "Loading all remotes...");
    for (int i = 0; i < nb_remotes; i++)
    {
        unsigned long id = this->REMOTE_BASE_ADDRESS + i;
        unsigned int rolling_code = 0;
        char* name = (char*)remote_names[i];
        bool enabled = false;
        unsigned short eeprom_address = i * sizeof(Remote);
        this->remotes.push_back(
            { id, rolling_code, name, enabled, eeprom_address });
    }
    this->update_remotes_from_memory();
    Logger::notice("RemotesContainer::load_remotes()", "Remotes loaded.");
};

/**
 * @brief PrettyPrint for all remotes through the Serial.
 *
 */
void RemotesContainer::print_remotes()
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
Vector<Remote> RemotesContainer::get_remotes() { return this->remotes; };

/**
 * @brief Update the remote inside the Vector and inside the memory if the
 * remote.id exists in the container.
 *
 * @param remote
 */
void RemotesContainer::update_remote(Remote remote) { this->update(remote); };

/**
 * @brief Reset the rolling code of the given remote. It will update this into
 * the saved remote.
 *
 * @param remote
 */
void RemotesContainer::reset_rolling_code(Remote remote)
{
    Logger::verbose("RemotesContainer::reset_rolling_code()",
        "Reseting remote rolling code...");
    remote.rolling_code = 0;
    this->update(remote);
    Logger::notice(
        "RemotesContainer::reset_rolling_code()", "Reset rolling code done.");
};

/**
 * @brief Reset the rolling code of the given remote (By its ID). It will
 * update this into the saved remote.
 *
 * @param remote_id
 */
void RemotesContainer::reset_rolling_code(unsigned long remote_id)
{
    Logger::verbose("RemotesContainer::reset_rolling_code()",
        "Reseting remote rolling code...");
    for (Remote remote : this->remotes)
    {
        if (remote.id != remote_id)
        {
            continue;
        }
        remote.rolling_code = 0;
        this->update(remote);
    }
    Logger::notice(
        "RemotesContainer::reset_rolling_code()", "Reset rolling code done.");
};

/**
 * @brief Toggle the state of the remote. If the remote is enabled, it will
 * be disabled. If the toggle is disabled, it will be enabled.
 *
 * @param remote
 */
void RemotesContainer::toggle_remote_enable(Remote remote)
{
    Logger::verbose("RemotesContainer::toggle_remote_enable()",
        "Toggleing remote enabled field.");
    for (Remote current_remote : this->remotes)
    {
        if (remote.id != current_remote.id)
        {
            continue;
        }
        current_remote.enabled = !current_remote.enabled;
        this->update(current_remote);
    }
    Logger::notice("RemotesContainer::toggle_remote_enable()",
        "Toggle remote enabled field done.");
};

/**
 * @brief Toggle the state of the remote. If the remote is enabled, it will
 * be disabled. If the toggle is disabled, it will be enabled.
 *
 * @param remote_id
 */
void RemotesContainer::toggle_remote_enable(unsigned long remote_id)
{
    Logger::verbose("RemotesContainer::toggle_remote_enable()",
        "Toggleing remote enabled field.");
    for (Remote remote : this->remotes)
    {
        if (remote.id != remote_id)
        {
            continue;
        }
        remote.enabled = !remote.enabled;
        this->update(remote);
    }
    Logger::notice("RemotesContainer::toggle_remote_enable()",
        "Toggle remote enabled field done.");
};

// PRIVATE

void RemotesContainer::update(Remote remote)
{
    Logger::notice("RemotesContainer::update()", "Updating the remote...");
    for (unsigned int i = 0; i < this->remotes.size(); i++)
    {
        if (this->remotes[i].id != remote.id)
        {
            continue;
        }

        Serial.print("Updating remote '");
        Serial.print(remote.id, HEX);
        Serial.println("'.");

        this->remotes[i].name = remote.name;
        this->remotes[i].rolling_code = remote.rolling_code;
        this->remotes[i].enabled = remote.enabled;

        EEPROM.put(this->remotes[i].eeprom_address, this->remotes[i]);
        EEPROM.commit();

        Logger::notice("RemotesContainer::update()", "Remote updated.");
        return;
    }
    Logger::warning("RemotesContainer::update()",
        "No remote found with id XXXXX. Nothing has been updated.");
    Serial.println(remote.id, HEX); // TODO include this value in the log
};

void RemotesContainer::update_remotes_from_memory()
{
    Logger::verbose("RemotesContainer::update_remotes_from_memory()",
        "Update all remotes with values stored in the memory.");
    for (unsigned int i = 0; i < this->remotes.size(); i++)
    {
        Remote saved_remote;
        EEPROM.get(this->remotes[i].eeprom_address, saved_remote);

        if (this->remotes[i].id != saved_remote.id)
        {
            Logger::warning("RemotesContainer::update_remotes_from_memory()",
                ""); // TODO logger
            Serial.print("Remote '");
            Serial.print(this->remotes[i].name);
            Serial.print("' at '");
            Serial.print(this->remotes[i].id, HEX);
            Serial.println("' seems to be corrupted into the memory.");
            Serial.println("The state of this remote will be re-writted.");

            EEPROM.put(this->remotes[i].eeprom_address, this->remotes[i]);
            EEPROM.commit();

            Serial.println("Remote writted into the memory.");
        }
        Logger::verbose("RemotesContainer::update_remotes_from_memory()",
            "Updating remote XXXXX with values from the memory..."); // TODO set remote ID.
        // update the current remote with EEPROM values
        this->remotes[i].name = saved_remote.name;
        this->remotes[i].enabled = saved_remote.enabled;
        this->remotes[i].rolling_code = saved_remote.rolling_code;
    }
};