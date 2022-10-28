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
#include "WIFISetup.h"
#include "Remotes.h"
#include "Serializer.h"

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
AsyncWebSocket ws("/ws");

Remote remotes_array[32];
static Vector<Remote> remotes(remotes_array);

RemotesManager manager(remotes);

byte frame[7];
byte checksum;

// ============================================================================
// WEBSERVER CALLBACKS
// ============================================================================

void home_page(AsyncWebServerRequest* request)
{
  Logger::notice("home_page()", "Home page requested.");
  request->send(LittleFS, "/index.html", String());
};

void css_file(AsyncWebServerRequest* request)
{
  request->send(LittleFS, "/static/css/main.css", "text/css");
};

void js_file(AsyncWebServerRequest* request)
{
  request->send(LittleFS, "/static/js/main.js", "text/javascript");
};

void get_remotes(AsyncWebServerRequest* request)
{
  Vector<Remote> all_remotes = manager.get_remotes();
  String content = Serializer::serialize(all_remotes);
  request->send(200, "application/json", content);
};

void blind_command(AsyncWebServerRequest* request)
{
  int remote_id = -1;
  int action = REMOTES_ACTIONS::UNKNOWN;

  for (unsigned int i = 0; i < request->params(); i++)
  {
    AsyncWebParameter* p_param = request->getParam(i);
    if (p_param->name() == "action")
    {
      String str_action = p_param->value();
      if (str_action == "up")
      {
        action = REMOTES_ACTIONS::UP;
      }
      else if (str_action == "down")
      {
        action = REMOTES_ACTIONS::DOWN;
      }
      else if (str_action == "stop")
      {
        action = REMOTES_ACTIONS::STOP;
      }
      else if (str_action == "prog")
      {
        action = REMOTES_ACTIONS::PROG;
      }
      else if (str_action == "reset")
      {
        action = REMOTES_ACTIONS::RESET;
      }
      else if (str_action == "enable")
      {
        action = REMOTES_ACTIONS::ENABLE;
      }
      else if (str_action == "disable")
      {
        action = REMOTES_ACTIONS::DISABLE;
      }
      else
      {
        action = REMOTES_ACTIONS::UNKNOWN;
      }
    }
    else if (p_param->name() == "remote_id")
    {
      String str_remote_id = p_param->value();
      remote_id = str_remote_id.toInt();
    }
    else
    {
      Logger::warning("blind_command()", "Unknown param.");
    }
  }

  if (remote_id < 0)
  {
    Logger::error("blind_command()", "Bad remote index.");
    request->send(400);
    return;
  }
  else if (action == REMOTES_ACTIONS::UNKNOWN)
  {
    Logger::error("blind_command()", "Unknown action.");
    request->send(400);
    return;
  }
  else
  {
    // All is valid, get remote.
    Remote* remote = manager.get_remote(remote_id);

    if (!remote)
    {
      Logger::error("blind_command()", "No remote found for the given id.");
      request->send(400);
      return;
    }

    if (action == REMOTES_ACTIONS::ENABLE || action == REMOTES_ACTIONS::DISABLE)
    {
      manager.toggle_remote_enable(remote->id);
      request->send(200);
      return;
    }

    if (!remote->enabled)
    {
      Logger::error("blind_command()", "Remote is not enabled.");
      request->send(400);
      return;
    }

    if (action == REMOTES_ACTIONS::RESET)
    {
      manager.reset_rolling_code(remote->id);
      request->send(200);
      return;
    }

    // We can send the command now!
    buildFrame(remote->id, remote->rolling_code, frame, action);

    Logger::notice("Sending command...");
    sendCommand(frame, 2);
    for (int i = 0; i < 2; i++)
    {
      sendCommand(frame, 7);
    }
    Logger::notice("Command sent.");
    manager.increment_rolling_code(remote->id);
  }
  request->send(200);
};

void not_found_page(AsyncWebServerRequest* request)
{
  Logger::error("not_found_page()", "Page not found.");
  request->send(404, "text/plain", "404: The content you are looking for was not found.");
};


// ============================================================================
// WEBSOCKET
// ============================================================================

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){

  if(type == WS_EVT_CONNECT){

    Serial.println("Websocket client connection received");
    client->text("Hello from ESP32 Server");

  } else if(type == WS_EVT_DISCONNECT){
    Serial.println("Client disconnected");

  }
}

// ============================================================================
// IMPLEMENTATIONS
// ============================================================================

void localLogger(Logger::Level level, const char* module, const char* message)
{
  String log = F("[")+ String(Logger::asString(level)) + F("]");
  if (strlen(module) > 0)
  {
    log += (F(": ") + String(module) + F(" "));
  }
  else{
    log += F(": ");
  }
  log += String(message);

  // Send to Serial
  Serial.println(log);
  // Send to WebSocket
  ws.textAll(log);
}

void buildFrame(unsigned long remote_id, unsigned int rolling_code, byte* frame, int action)
{
  byte button;
  switch (action)
  {
  case REMOTES_ACTIONS::UP:
    Logger::verbose("buildFrame()", "Building frame for UP Action...");
    button = BYTE_ACTION_UP;
    break;
  case REMOTES_ACTIONS::DOWN:
    Logger::verbose("buildFrame()", "Building frame for DOWN Action...");
    button = BYTE_ACTION_DOWN;
    break;
  case REMOTES_ACTIONS::STOP:
    Logger::verbose("buildFrame()", "Building frame for STOP Action...");
    button = BYTE_ACTION_STOP;
    break;
  case REMOTES_ACTIONS::PROG:
    Logger::verbose("buildFrame()", "Building frame for PROG Action...");
    button = BYTE_ACTION_PROG;
    break;
  default:
    // Stop by default
    Logger::verbose("buildFrame()", "Building frame for STOP Action...");
    button = BYTE_ACTION_STOP;
    break;
  }

  frame[0] = 0xA7;
  frame[1] = button << 4;
  frame[2] = rolling_code >> 8;
  frame[3] = rolling_code;
  frame[4] = remote_id >> 16;
  frame[5] = remote_id >> 8;
  frame[6] = remote_id;

  byte checksum = 0;
  for (byte i = 0; i < 7; i++)
  {
    checksum = checksum ^ frame[i] ^ (frame[i] >> 4);
  }
  checksum &= 0b1111;

  frame[1] |= checksum;

  for (byte i = 1; i < 7; i++)
  {
    frame[i] ^= frame[i - 1];
  }

  Logger::verbose("buildFrame()", "Frame builded.");
};

void sendCommand(byte* frame, byte sync)
{
  if (sync == 2)
  {
    // Only with the first frame.
    // Wake-up pulse & Silence
    SIG_HIGH;
    delayMicroseconds(9415);
    SIG_LOW;
    delayMicroseconds(89565);
  }

  // Hardware sync: two sync for the first frame, seven for the following
  // ones.
  for (int i = 0; i < sync; i++)
  {
    SIG_HIGH;
    delayMicroseconds(4 * SYMBOL);
    SIG_LOW;
    delayMicroseconds(4 * SYMBOL);
  }

  // Software sync
  SIG_HIGH;
  delayMicroseconds(4550);
  SIG_LOW;
  delayMicroseconds(SYMBOL);

  // Data: bits are sent one by one, starting with the MSB.
  for (byte i = 0; i < 56; i++)
  {
    if (((frame[i / 8] >> (7 - (i % 8))) & 1) == 1)
    {
      SIG_LOW;
      delayMicroseconds(SYMBOL);
      SIG_HIGH;
      delayMicroseconds(SYMBOL);
    }
    else
    {
      SIG_HIGH;
      delayMicroseconds(SYMBOL);
      SIG_LOW;
      delayMicroseconds(SYMBOL);
    }
  }

  SIG_LOW;
  delayMicroseconds(30415); // Inter-frame silence
};

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
  Logger::setOutputFunction(localLogger);

  // Open the output for 433.42MHz and 433.92MHz transmitter
  pinMode(PORT_TX, OUTPUT);
  SIG_LOW;
  digitalWrite(PORT_TX, LOW);

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
  manager.load_remotes(SOMFY_CONFIG_REMOTES, nb_remotes);

  // Routes setup
  server.on("/", HTTP_GET, home_page);
  server.on("/static/css/main.css", HTTP_GET, css_file);
  server.on("/static/js/main.js", HTTP_GET, js_file);

  server.on("/blind", HTTP_GET, blind_command);
  server.on("/remotes", HTTP_GET, get_remotes);
  server.onNotFound(not_found_page);

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  // Start the server
  server.begin();

  // Vector<Remote> remotes = manager.get_remotes();
  // auto salon_remote = remotes[0];
  // salon_remote.rolling_code += 1;
  // salon_remote.enabled = true;
  // manager.update_remote(salon_remote);
  // manager.reset_rolling_code(salon_remote.id);
  // manager.toggle_remote_enable(salon_remote);

  Logger::notice("setup()", "Setup done.");

  
};

void loop() {};
