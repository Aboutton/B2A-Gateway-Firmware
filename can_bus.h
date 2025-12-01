#ifndef CAN_BUS_H
#define CAN_BUS_H

#include <Arduino.h>
#include <SPI.h>
#include <ACAN2517FD.h>
#include "config.h"

// Global CAN objects
extern ACAN2517FD can1;
extern ACAN2517FD can2;

// CAN functions
void setupCAN();
void configureMCP_GPIO0(uint8_t cs);
bool canSend(uint8_t bus, uint32_t id, bool ext, const uint8_t* data, uint8_t dlc);
void pollCAN();

#endif