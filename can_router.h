#ifndef CAN_ROUTER_H
#define CAN_ROUTER_H

#include <Arduino.h>
#include <ACAN2517FD.h>
#include "gateway_config.h"

void routeMessage(uint8_t src_bus, const CANFDMessage& msg);

#endif
