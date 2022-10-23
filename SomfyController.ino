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
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
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

AsyncWebServer server(SERVER_PORT);

Remote remotes_array[32];
static Vector<Remote> remotes(remotes_array);

RemotesManager container(remotes);

// ============================================================================
// WEBSERVER CALLBACKS
// ============================================================================

void home_page(AsyncWebServerRequest* request)
{
    Logger::notice("home_page()", "Home page requested.");
    request->send(LittleFS, "/index.html", String());
    container.print_remotes();
};

void css_file(AsyncWebServerRequest* request)
{
    request->send(LittleFS, "/style.css", "text/css");
};

void blind_up(AsyncWebServerRequest* request)
{
    Logger::notice("blind_up()", "Up command requested.");
    int nb_params = request->params();
    for (int i = 0; i < nb_params; i++)
    {
        AsyncWebParameter* p = request->getParam(i);
        Serial.println(p->value());
    }
    request->send(200, "text/plain", "UP");
};

void blind_down(AsyncWebServerRequest* request)
{
    Logger::notice("blind_down()", "Down command requested.");
    int nb_params = request->params();
    request->send(200, "text/plain", "DOWN");
};

void blind_stop(AsyncWebServerRequest* request)
{
    Logger::notice("blind_stop()", "Stop command requested.");
    int nb_params = request->params();
    request->send(200, "text/plain", "STOP");
};

void blind_prog(AsyncWebServerRequest* request)
{
    Logger::notice("blind_prog()", "Prog command requested.");
    int nb_params = request->params();
    request->send(200, "text/plain", "PROG");
};

void not_found_page(AsyncWebServerRequest* request)
{
    Logger::error("not_found_page()", "Page not found.");
    request->send(404, "text/plain", "404: The content you are looking for was not found.");
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

    // SPIFFS Setup
    if (!LittleFS.begin())
    {
        Logger::error("setup()", "An Error has occurred while mounting SPIFFS.");
        return;
    }

    // WIFI Setup
    setup_wifi(SSID, PASSWORD);

    // Load remotes
    int nb_remotes = sizeof(SOMFY_CONFIG_REMOTES) / sizeof(SOMFY_CONFIG_REMOTES[0]);
    container.load_remotes(SOMFY_CONFIG_REMOTES, nb_remotes);

    // Routes setup
    server.on("/", HTTP_GET, home_page);
    server.on("/style.css", HTTP_GET, css_file);
    server.on("/blind/up", HTTP_GET, blind_up);
    server.on("/blind/down", HTTP_GET, blind_down);
    server.on("/blind/stop", HTTP_GET, blind_stop);
    server.on("/blind/prog", HTTP_GET, blind_prog);
    server.onNotFound(not_found_page);

    // Start the server
    server.begin();

    // Vector<Remote> remotes = container.get_remotes();
    // auto salon_remote = remotes[0];
    // salon_remote.rolling_code += 1;
    // salon_remote.enabled = true;
    // container.update_remote(salon_remote);
    // container.reset_rolling_code(salon_remote.id);
    // container.toggle_remote_enable(salon_remote);

    Logger::notice("setup()", "Setup done.");
};

void loop() {};
