/**
 * @file SomfyController.ino
 * @author Laurette Alexandre
 * @brief Control SOMFY blinds.
 * @version 0.1
 * @date 2022-10-20
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <uri/UriBraces.h>

#include "config.h"
#include "wifi_setup.h"
#include "remotes.h"

#define PORT_TX D1

#define SYMBOL 640
#define BYTE_ACTION_UP 0x2
#define BYTE_ACTION_STOP 0x1
#define BYTE_ACTION_DOWN 0x4
#define BYTE_ACTION_PROG 0x8

#define REMOTES_EEPROM_ADDRESS 0

#define SIG_HIGH GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, 1 << PORT_TX)
#define SIG_LOW GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, 1 << PORT_TX)

// ============================================================================
// GLOBAL VARS
// ============================================================================

ESP8266WebServer web_server(SERVER_PORT);

// ============================================================================
// IMPLEMENTATIONS
// ============================================================================

void home_page()
{
    Serial.println("INDEX");
    web_server.send(200, "text/plain", "Page d'accueil");
}

void blind_up()
{
    Serial.println("UP");
    Serial.println(web_server.pathArg(0));
    web_server.send(200, "text/plain", "UP");
}

void blind_down()
{
    Serial.println("DOWN");
    Serial.println(web_server.pathArg(0));
    web_server.send(200, "text/plain", "DOWN");
}

void blind_stop()
{
    Serial.println("STOP");
    Serial.println(web_server.pathArg(0));
    web_server.send(200, "text/plain", "STOP");
}

void blind_prog()
{
    Serial.println("PROG");
    Serial.println(web_server.pathArg(0));
    web_server.send(200, "text/plain", "PROG");
}

void not_found_page() { web_server.send(404, "text/plain", "404: Not found"); }

// ============================================================================
// ENTRYPOINTS
// ============================================================================

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        continue;

    // WIFI Setup
    setup_wifi(SSID, PASSWORD);

    // Routes setup
    web_server.on("/", home_page);
    web_server.on(UriBraces("/blind/{}/up"), blind_up);
    web_server.on(UriBraces("/blind/{}/down"), blind_down);
    web_server.on(UriBraces("/blind/{}/stop"), blind_stop);
    web_server.on(UriBraces("/blind/{}/prog"), blind_prog);
    web_server.onNotFound(not_found_page);
    // Start the web server
    web_server.begin();
}

void loop() { web_server.handleClient(); }