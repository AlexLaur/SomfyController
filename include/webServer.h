#pragma once

#include <ESPAsyncWebServer.h>

#include <config.h>
#include <observer.h>
#include <controller.h>

class WebServer : public Observer
{
  public:
  WebServer(const unsigned short port, Controller* controller);
  ~WebServer();
  static WebServer* getInstance();

  void setup();
  void begin();

  void notified(const char* action, const char* data); // from observer

  private:
  static WebServer* m_instance;
  AsyncWebServer* m_server = nullptr;
  Controller* m_controller = nullptr;

  // API REST
  static void handleSystemRestart(AsyncWebServerRequest* request);
  static void handleFetchSystemInfos(AsyncWebServerRequest* request);
  static void handleFetchWifiNetworks(AsyncWebServerRequest* request);
  static void handleFetchWifiConfiguration(AsyncWebServerRequest* request);
  static void handleUpdateWifiConfiguration(AsyncWebServerRequest* request);
  static void handleFetchMQTTConfiguration(AsyncWebServerRequest* request);
  static void handleUpdateMQTTConfiguration(AsyncWebServerRequest* request);
  static void handleFetchAllRemotes(AsyncWebServerRequest* request);
  static void handleFetchRemote(AsyncWebServerRequest* request);
  static void handleCreateRemote(AsyncWebServerRequest* request);
  static void handleUpdateRemote(AsyncWebServerRequest* request);
  static void handleDeleteRemote(AsyncWebServerRequest* request);
  static void handleActionRemote(AsyncWebServerRequest* request);
  // HTML
  static void handleHTMLHomePage(AsyncWebServerRequest* request);
  static void handleHTMLNotFoundPage(AsyncWebServerRequest* request);
};