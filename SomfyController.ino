/**
 * @file SomfyController.ino
 * @author Laurette Alexandre
 * @brief Control SOMFY blinds.
 * @version 2.0.0
 * @date 2024-06-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#define DEBUGLOG_DEFAULT_LOG_LEVEL_DEBUG

#include <DebugLog.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    continue;

  // Wait one second to avoid bad chars in serial
  delay(1000);

  LOG_INFO("Trying WiFi connection...");
};

void loop() {};