/**
 * @file Serializer.cpp
 * @author Laurette Alexandre
 * @brief Implementation of Serializer.
 * Serialize all remotes to a JSON object.
 * @version 0.1
 * @date 2022-10-27
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <ArduinoJson.h>
#include <Vector.h>

#include "Serializer.h"
#include "Remotes.h"

String Serializer::serialize(Vector<Remote> remotes)
{
  String output;
  StaticJsonDocument<1024> doc;
  JsonArray array = doc.to<JsonArray>();

  for (Remote remote : remotes)
  {
    JsonObject object = array.createNestedObject();
    object["id"] = remote.id;
    object["name"] = remote.name;
    object["rolling_code"] = remote.rolling_code;
    object["enabled"] = remote.enabled;
  }
  serializeJson(doc, output);
  return output;
};