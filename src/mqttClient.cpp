/**
 * @file mqttClient.cpp
 * @author Laurette Alexandre
 * @brief Implementation for MQTT client.
 * @version 2.1.0
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
#include <Arduino.h>
#include <DebugLog.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <config.h>
#include <result.h>
#include <controller.h>
#include <mqttClient.h>
#include <mqttConfig.h>

WiFiClient espClient;
PubSubClient pubSubClient(espClient);

MQTTClient* MQTTClient::m_instance = nullptr;

MQTTClient::MQTTClient(Controller* controller)
    : m_controller(controller)
{
  this->m_instance = this;
}

MQTTClient* MQTTClient::getInstance() { return MQTTClient::m_instance; }

bool MQTTClient::connect(const MQTTConfiguration& conf)
{
  pubSubClient.setServer(conf.broker, conf.port);
  String clientId = this->getClientIdentifier();

  if (pubSubClient.connect(clientId.c_str(), conf.username, conf.password))
  {
    LOG_INFO("MQTT client started.");

    pubSubClient.setCallback(MQTTClient::receive);
    pubSubClient.publish(
        "esprtsomfy/system/infos", this->m_controller->fetchSystemInfos().data.c_str());
    pubSubClient.publish(
        "esprtsomfy/remotes/list", this->m_controller->fetchAllRemotes().data.c_str());
    pubSubClient.subscribe("inTopic");
  }
  else
  {
    LOG_ERROR("Cannot connect to the MQTT broker.");
    return false;
  }
  return true;
}

void MQTTClient::handleMessages()
{
  if (!pubSubClient.connected())
  {
    return;
  }
  pubSubClient.loop();
}

bool MQTTClient::isConnected() { return pubSubClient.connected(); }

void MQTTClient::notified(const char* action, const char* data)
{
  if (!this->isConnected())
  {
    LOG_ERROR("MQTT Client is not connected. Nothing can be published.");
    return;
  }
  // if (strcmp(action, "remote-up") == 0){
  //     LOG_INFO("Remote UP command catched.");
  //     pubSubClient.publish("esprtsomfy/remotes/action/up", data);
  // }
  // else if (strcmp(action, "remote-delete") == 0){
  //     LOG_INFO("Remote Delete command catched.");
  //     Result result = this->m_controller->fetchAllRemotes();
  //     pubSubClient.publish("esprtsomfy/remotes/list", result.data.c_str(), true);
  // }
  // else if (strcmp(action, "remote-create") == 0){
  //     LOG_INFO("Remote Create command catched.");
  //     Result result = this->m_controller->fetchAllRemotes();
  //     pubSubClient.publish("esprtsomfy/remotes/list", result.data.c_str(), true);
  // }
}

// PRIVATE
void MQTTClient::receive(const char* topic, byte* payload, uint32_t length)
{
  LOG_INFO("Message arrived on topic: ", topic);

  MQTTClient* instance = MQTTClient::getInstance();

  for (unsigned int i = 0; i < length; i++)
  {
    LOG_INFO((char)payload[i]);
  }

  // // Switch on the LED if an 1 was received as first character
  // if ((char)payload[0] == '1') {
  //   digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
  //   // but actually the LED is on; this is because
  //   // it is active low on the ESP-01)
  // } else {
  //   digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  // }
}

String MQTTClient::getClientIdentifier()
{
  String clientId = String(APP_NAME);
  clientId += String(random(0xffff), HEX);
  return clientId;
}