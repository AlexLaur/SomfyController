#pragma once

#include <Arduino.h>
#include <PubSubClient.h>

#include <observer.h>
#include <controller.h>
#include <mqttConfig.h>

void callback(char* topic, byte* payload, unsigned int length);

class MQTTClient : public Observer
{
  public:
  MQTTClient(Controller* controller);
  bool connect(const MQTTConfiguration& conf);
  void handleMessages();
  bool isConnected();

  void notified(const char* action, const char* data); // from observer

  private:
  Controller* m_controller = nullptr;
  static void receive(const char* topic, byte* payload, uint32_t length);
  String getClientIdentifier();
};