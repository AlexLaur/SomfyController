#pragma once

class Transmitter
{
  public:
  virtual bool sendUpCmd(unsigned long remoteId, unsigned int rollingCode) = 0;
  virtual bool sendStopCmd(unsigned long remoteId, unsigned int rollingCode) = 0;
  virtual bool sendDownCmd(unsigned long remoteId, unsigned int rollingCode) = 0;
  virtual bool sendProgCmd(unsigned long remoteId, unsigned int rollingCode) = 0;
};