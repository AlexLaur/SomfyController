#include <ESP8266WiFi.h>
#include <Logger.h>

#include "wifi_setup.h"

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
    snprintf_P(s, sizeof(s), PSTR("IP address: %d.%d.%d.%d"), ip[0], ip[1],
        ip[2], ip[3]);
    Logger::notice("setup_wifi()", s);
};