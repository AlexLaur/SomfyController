#pragma once

#include <Arduino.h>
#include "abstracts/transmitter.h"

class RTSTransmitter : public Transmitter
{
  public:
  void init();
  bool sendUpCmd(unsigned long remoteId, unsigned int rollingCode);
  bool sendStopCmd(unsigned long remoteId, unsigned int rollingCode);
  bool sendDownCmd(unsigned long remoteId, unsigned int rollingCode);
  bool sendProgCmd(unsigned long remoteId, unsigned int rollingCode);

  private:
  byte m_frame[7];

  void buildFrame(unsigned long remoteId, unsigned int rollingCode, byte action);
  void sendCommand(); // Will call sendCommand(sync)
  void sendCommand(byte sync);
  void debugBuildedFrame(int base);
};