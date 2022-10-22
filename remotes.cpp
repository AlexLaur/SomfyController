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

#include "remotes.h"

const unsigned long RemotesContainer::REMOTE_BASE_ADDRESS = 0x100000;

/**
 * @brief Construct a new Remotes Container:: Remotes Container object
 *
 * @param remotes
 */
RemotesContainer::RemotesContainer(Vector<Remote> remotes)
{
    EEPROM.begin(512);
    this->remotes_ = remotes;
};

/**
 * @brief Destroy the Remotes Container:: Remotes Container object
 *
 */
RemotesContainer::~RemotesContainer() { EEPROM.end(); }

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
    for (int i = 0; i < nb_remotes; i++)
    {
        unsigned long id = this->REMOTE_BASE_ADDRESS + i;
        unsigned int rolling_code = 0;
        char* name = (char*)remote_names[i];
        bool enabled = false;
        unsigned short eeprom_address = i * sizeof(Remote);
        this->remotes_.push_back(
            { id, rolling_code, name, enabled, eeprom_address });
    }
    this->update_remotes_from_memory();
};

/**
 * @brief PrettyPrint for all remotes through the Serial.
 *
 */
void RemotesContainer::print_remotes()
{
    for (Remote remote : this->remotes_)
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
Vector<Remote> RemotesContainer::get_remotes() { return this->remotes_; };

/**
 * @brief Update the remote inside the Vector and inside the memory if the
 * remote.id exists in the container.
 *
 * @param remote
 */
void RemotesContainer::update_remote(Remote remote)
{
    for (unsigned int i = 0; i < this->remotes_.size(); i++)
    {
        if (this->remotes_[i].id != remote.id)
        {
            continue;
        }

        Serial.print("Updating remote '");
        Serial.print(remote.id, HEX);
        Serial.println("'.");

        this->remotes_[i].name = remote.name;
        this->remotes_[i].rolling_code = remote.rolling_code;
        this->remotes_[i].enabled = remote.enabled;

        EEPROM.put(this->remotes_[i].eeprom_address, this->remotes_[i]);
        EEPROM.commit();

        Serial.println("Remote updated.");
        return;
    }
    Serial.println("No remote found with id '");
    Serial.print(remote.id, HEX);
    Serial.println("'. Nothing has been updated.");
};

/**
 * @brief Reset the rolling code of the given remote. It will update this into
 * the saved remote.
 *
 * @param remote
 */
void RemotesContainer::reset_rolling_code(Remote remote) {

};

// PRIVATE

void RemotesContainer::update_remotes_from_memory()
{
    for (unsigned int i = 0; i < this->remotes_.size(); i++)
    {
        Remote saved_remote;
        EEPROM.get(this->remotes_[i].eeprom_address, saved_remote);

        if (this->remotes_[i].id != saved_remote.id)
        {
            Serial.print("Remote '");
            Serial.print(this->remotes_[i].name);
            Serial.print("' at '");
            Serial.print(this->remotes_[i].id, HEX);
            Serial.println("' seems to be corrupted into the memory.");
            Serial.println("The state of this remote will be re-writted.");

            EEPROM.put(this->remotes_[i].eeprom_address, this->remotes_[i]);
            EEPROM.commit();

            Serial.println("Remote writted into the memory.");
        }
        // update the current remote with EEPROM values
        this->remotes_[i].name = saved_remote.name;
        this->remotes_[i].enabled = saved_remote.enabled;
        this->remotes_[i].rolling_code = saved_remote.rolling_code;
    }
};