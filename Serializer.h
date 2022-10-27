/**
 * @file Serializer.h
 * @author Laurette Alexandre
 * @brief Header for serializer
 * @version 0.1
 * @date 2022-10-27
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <ArduinoJson.h>
#include <Vector.h>
#include "Remotes.h"

class Serializer
{
  public:
  static String serialize(Vector<Remote> remotes);
};