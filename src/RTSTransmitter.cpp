#include <Arduino.h>
#include <DebugLog.h>
#include "RTSTransmitter.h"

#define PORT_TX D1

#define SYMBOL 640
#define BYTE_ACTION_UP 0x2
#define BYTE_ACTION_STOP 0x1
#define BYTE_ACTION_DOWN 0x4
#define BYTE_ACTION_PROG 0x8

#define SIG_HIGH GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, 1 << PORT_TX)
#define SIG_LOW GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, 1 << PORT_TX)

byte frame[7];
byte checksum;

void RTSTransmitter::init()
{
  pinMode(PORT_TX, OUTPUT);
  SIG_LOW;
  digitalWrite(PORT_TX, LOW);
}

bool RTSTransmitter::sendUpCmd(unsigned long remoteId, unsigned int rollingCode)
{
  this->buildFrame(remoteId, rollingCode, frame, BYTE_ACTION_UP);
  this->sendCommand(frame, 2);
  for (int i = 0; i < 2; i++)
  {
    this->sendCommand(frame, 7);
  }
  return true;
};
bool RTSTransmitter::sendStopCmd(unsigned long remoteId, unsigned int rollingCode)
{
  this->buildFrame(remoteId, rollingCode, frame, BYTE_ACTION_STOP);
  this->sendCommand(frame, 2);
  for (int i = 0; i < 2; i++)
  {
    this->sendCommand(frame, 7);
  }
  return true;
};
bool RTSTransmitter::sendDownCmd(unsigned long remoteId, unsigned int rollingCode)
{
  this->buildFrame(remoteId, rollingCode, frame, BYTE_ACTION_DOWN);
  this->sendCommand(frame, 2);
  for (int i = 0; i < 2; i++)
  {
    this->sendCommand(frame, 7);
  }
  return true;
};
bool RTSTransmitter::sendProgCmd(unsigned long remoteId, unsigned int rollingCode)
{
  this->buildFrame(remoteId, rollingCode, frame, BYTE_ACTION_PROG);
  this->sendCommand(frame, 2);
  for (int i = 0; i < 2; i++)
  {
    this->sendCommand(frame, 7);
  }
  return true;
};

// PRIVATE
void RTSTransmitter::buildFrame(
    unsigned long remoteId, unsigned int rollingCode, byte* frame, byte action)
{
  frame[0] = 0xA7;
  frame[1] = action << 4;
  frame[2] = rollingCode >> 8;
  frame[3] = rollingCode;
  frame[4] = remoteId >> 16;
  frame[5] = remoteId >> 8;
  frame[6] = remoteId;

  byte checksum = 0;
  for (byte i = 0; i < 7; i++)
  {
    checksum = checksum ^ frame[i] ^ (frame[i] >> 4);
  }
  checksum &= 0b1111;

  frame[1] |= checksum;

  for (byte i = 1; i < 7; i++)
  {
    frame[i] ^= frame[i - 1];
  }

  // For debug
  this->debugFrame(frame, HEX);
  this->debugFrame(frame, BIN);
  LOG_DEBUG("Frame builded.");
};

void RTSTransmitter::sendCommand(byte* frame, byte sync)
{
  if (sync == 2)
  {
    // Only with the first frame.
    // Wake-up pulse & Silence
    SIG_HIGH;
    delayMicroseconds(9415);
    SIG_LOW;
    delayMicroseconds(89565);
  }

  // Hardware sync: two sync for the first frame, seven for the following
  // ones.
  for (int i = 0; i < sync; i++)
  {
    SIG_HIGH;
    delayMicroseconds(4 * SYMBOL);
    SIG_LOW;
    delayMicroseconds(4 * SYMBOL);
  }

  // Software sync
  SIG_HIGH;
  delayMicroseconds(4550);
  SIG_LOW;
  delayMicroseconds(SYMBOL);

  // Data: bits are sent one by one, starting with the MSB.
  for (byte i = 0; i < 56; i++)
  {
    if (((frame[i / 8] >> (7 - (i % 8))) & 1) == 1)
    {
      SIG_LOW;
      delayMicroseconds(SYMBOL);
      SIG_HIGH;
      delayMicroseconds(SYMBOL);
    }
    else
    {
      SIG_HIGH;
      delayMicroseconds(SYMBOL);
      SIG_LOW;
      delayMicroseconds(SYMBOL);
    }
  }

  SIG_LOW;
  delayMicroseconds(30415); // Inter-frame silence
};


void RTSTransmitter::debugFrame(byte* frame, int base){
    String debugFrame;
  for (int i = 0; i < 7; i++)
  {
    debugFrame += String(frame[i], base);
    debugFrame += " ";
  }
  debugFrame.toUpperCase();
  LOG_DEBUG("Frame:", debugFrame);
}