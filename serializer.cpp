#include <ArduinoJson.h>
#include <Vector.h>

#include "serializer.h"
#include "remotes.h"


String Serializer::serialize(Vector<Remote> remotes)
{
    String output;
    StaticJsonDocument<1024> doc;
    JsonArray array = doc.to<JsonArray>();

    for (Remote remote : remotes){
        JsonObject object = array.createNestedObject();
        object["id"] = remote.id;
        object["name"] = remote.name;
        object["rolling_code"] = remote.rolling_code;
        object["enabled"] = remote.enabled;
    }
    serializeJson(doc, output);
    return output;
};