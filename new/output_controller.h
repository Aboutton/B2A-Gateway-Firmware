#ifndef OUTPUT_CONTROLLER_H
#define OUTPUT_CONTROLLER_H

#include <Arduino.h>
#include <ACAN2517FD.h>
#include "gateway_config.h"
#include "config.h"

void initOutputController();
void processOutputTriggers(uint8_t bus, const CANFDMessage& msg);
void updateOutputsFromDigitalInputs();
void applyAlwaysOnOutputs();

#endif
