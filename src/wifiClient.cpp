#include <DebugLog.h>
#include <ESP8266WiFi.h>

#include "wifiClient.h"
#include "dto/networks.h"

/**
 * @brief Connect this device to a network
 *
 * @param conf The NetworkConfiguration to use
 * @return true, if the device is connected to the network
 * @return false, otherwise
 */
bool WifiClient::connect(const NetworkConfiguration& conf)
{
  return this->connect(conf.ssid, conf.password);
}

/**
 * @brief Connect this device to a network
 *
 * @param ssid The SSID of the network to connect to
 * @param password The password of the network
 * @return true, if the device is connected to the network
 * @return false, otherwise
 */
bool WifiClient::connect(const char* ssid, const char* password)
{
  LOG_DEBUG("Trying to connect to the WiFi...");
  LOG_DEBUG("SSID provided: ", ssid);
  LOG_DEBUG("Password provided: ", password);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED && millis() < 15000)
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    LOG_DEBUG(".");
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    LOG_INFO("WiFi connected.");
    return true;
  }
  else
  {
    LOG_ERROR("Failed to connect to the Wifi. Canceling...");
    WiFi.disconnect();
    return false;
  }
};

/**
 * @brief Get the IP Address of this device
 *
 * @return IPAddress
 */
IPAddress WifiClient::getIP() { return WiFi.localIP(); };

/**
 * @brief Is Connected to a network ?
 *
 * @return true, if connected
 * @return false, otherwise
 */
bool WifiClient::isConnected() { return (WiFi.status() == WL_CONNECTED); };

/**
 * @brief Return the list of Network found
 * Don't forget to free the result of this method.
 *
 * > auto networks = WifiClient.getNetworks();
 * > ...
 * > free(networks);
 *
 * @return Network*
 */
Network* WifiClient::getNetworks()
{
  int n = WiFi.scanNetworks();
  Network* networks = (Network*)malloc(n * sizeof(Network));
  if (n == 0)
  {
    LOG_WARN("No Wifi Networks detected.");
    return networks;
  }

  for (int i = 0; i < n; ++i)
  {
    // Get SSID and RSSI for each network found
    String ssid = WiFi.SSID(i); // SSID
    long rssi = WiFi.RSSI(i); // Signal strength in dBm
    Network network = { ssid, rssi };
    networks[i] = network;
  }

  return networks;
};