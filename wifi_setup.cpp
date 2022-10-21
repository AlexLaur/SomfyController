#include <ESP8266WiFi.h>

#include "wifi_setup.h"

/**
 * @brief Set the up wifi object
 *
 * @param ssid
 * @param password
 */
void setup_wifi(const char *ssid, const char *password)
{
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("-----------------------");
};