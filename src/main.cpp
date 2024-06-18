/**
 * @file SomfyController.ino
 * @author Laurette Alexandre
 * @brief Control SOMFY blinds.
 * @version 2.0.0
 * @date 2024-06-06
 *
 * @copyright (c) 2024 Laurette Alexandre
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#define DEBUGLOG_DEFAULT_LOG_LEVEL_DEBUG

#include <Arduino.h>
#include <DebugLog.h>
#include <LittleFS.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <config.h>
#include <remote.h>
#include <networks.h>
#include <controller.h>
#include <wifiClient.h>
#include <wifiAccessPoint.h>
#include <RTSTransmitter.h>
#include <eepromDatabase.h>
#include <jsonSerializer.h>


void setup()
{
  Serial.begin(115200);
  while (!Serial)
    continue;
  // Wait one second to avoid bad chars in serial
  delay(5000);
}

void loop()
{
  // put your main code here, to run repeatedly:
}