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
#include <Vector.h>
#include <Logger.h>

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

Remote remotes_array[32];
static Vector<Remote> remotes(remotes_array);

RemotesManager container(remotes);

// ============================================================================
// WEBSERVER CALLBACKS
// ============================================================================

void home_page()
{
    Logger::notice("home_page()", "Home page requested.");
    web_server.send(200, "text/plain", "Page d'accueil");
    container.print_remotes();
};

void blind_up()
{
    Logger::notice("blind_up()", "Up command requested.");
    Serial.println(web_server.pathArg(0));
    web_server.send(200, "text/plain", "UP");
};

void blind_down()
{
    Logger::notice("blind_down()", "Down command requested.");
    Serial.println(web_server.pathArg(0));
    web_server.send(200, "text/plain", "DOWN");
};

void blind_stop()
{
    Logger::notice("blind_stop()", "Stop command requested.");
    Serial.println(web_server.pathArg(0));
    web_server.send(200, "text/plain", "STOP");
};

void blind_prog()
{
    Logger::notice("blind_prog()", "Prog command requested.");
    Serial.println(web_server.pathArg(0));
    web_server.send(200, "text/plain", "PROG");
};

void not_found_page()
{
    Logger::error("not_found_page()", "Page not found.");
    web_server.send(404, "text/plain", "404: Not found");
};

// ============================================================================
// IMPLEMENTATIONS
// ============================================================================

// ============================================================================
// ENTRYPOINTS
// ============================================================================

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        continue;

    // Wait one second to avoid bad chars in serial
    delay(1000);

    // Logger setup
    Logger::setLogLevel(Logger::VERBOSE);

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

    // Load remotes
    int nb_remotes = sizeof(SOMFY_CONFIG_REMOTES) / sizeof(SOMFY_CONFIG_REMOTES[0]);
    container.load_remotes(SOMFY_CONFIG_REMOTES, nb_remotes);

    Logger::notice("setup()", "Setup done.");

    Vector<Remote> remotes = container.get_remotes();
    auto salon_remote = remotes[0];
    // salon_remote.rolling_code += 1;
    // container.update_remote(salon_remote);
    // container.reset_rolling_code(salon_remote.id);
    // container.toggle_remote_enable(salon_remote);
};

void loop() { web_server.handleClient(); };