/**
 * @file WIFISetup.cpp
 * @author Laurette Alexandre
 * @brief Implementation for WIFISetup
 * @version 0.1
 * @date 2022-10-27
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <ESP8266WiFi.h>
#include <Logger.h>

#include "WIFISetup.h"

/**
 * @brief Set the up wifi object
 *
 * @param ssid
 * @param password
 */
void setup_wifi(const char* ssid, const char* password)
{
    char s[32]; // used for logger messages
    snprintf_P(s, sizeof(s), PSTR("Connecting to: %s"), ssid);
    Logger::notice("setup_wifi()", s);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("connected.");

    IPAddress ip = WiFi.localIP();
    snprintf_P(s, sizeof(s), PSTR("IP address: %d.%d.%d.%d"), ip[0], ip[1], ip[2], ip[3]);
    Logger::notice("setup_wifi()", s);
};