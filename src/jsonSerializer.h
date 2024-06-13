/**
 * @file jsonSerializer.h
 * @author Laurette Alexandre
 * @brief Header for JSON serialization.
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
#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "dto/remote.h"
#include "dto/networks.h"
#include "abstracts/serializer.h"

class JSONSerializer : public Serializer
{
  public:
  String serializeRemote(const Remote& remote);
  String serializeRemotes(const Remote remotes[], int size);
  String serializeNetworkConfig(const NetworkConfiguration& networkConfig);
  String serializeNetworks(const Network networks[], int size);

  private:
    void serializeRemote(JsonObject object, const Remote& remote);
};