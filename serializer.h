#pragma once

#include <ArduinoJson.h>
#include <Vector.h>
#include "remotes.h"

class Serializer
{
public:
    static String serialize(Vector<Remote> remotes);
};