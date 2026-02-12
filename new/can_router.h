#ifndef CAN_ROUTER_H
#define CAN_ROUTER_H

#include <Arduino.h>
#include <ACAN2517FD.h>
#include "gateway_config.h"

// Build pre-sorted route index — call after config load or route change
void buildRouteIndex();

// Route a received CAN message through matching rules
void routeMessage(uint8_t src_bus, const CANFDMessage& msg);

// Per-route statistics
void getRouteStats(uint8_t index, uint32_t& forwarded, uint32_t& dropped);
void resetRouteStats();

#endif
