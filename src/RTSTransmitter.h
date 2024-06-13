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
  byte m_checksum;

  void buildFrame(unsigned long remoteId, unsigned int rollingCode, byte* frame, byte action);
  void sendCommand(byte* frame, byte sync);
  void debugFrame(byte* frame, int base);
};