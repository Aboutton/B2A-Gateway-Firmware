#include "can_router.h"
#include "can_bus.h"

void routeMessage(uint8_t src_bus, const CANFDMessage& msg) {
  for (uint8_t i = 0; i < gw_config.route_count && i < 40; i++) {
    CanRoute& route = gw_config.routes[i];
    
    if (!route.enabled) continue;
    
    // Check direction
    bool should_route = false;
    uint8_t dst_bus = 0;
    
    if (src_bus == 1 && (route.direction == ROUTE_CAN1_TO_CAN2 || route.direction == ROUTE_BIDIRECTIONAL)) {
      should_route = true;
      dst_bus = 2;
    } else if (src_bus == 2 && (route.direction == ROUTE_CAN2_TO_CAN1 || route.direction == ROUTE_BIDIRECTIONAL)) {
      should_route = true;
      dst_bus = 1;
    }
    
    if (!should_route) continue;
    
    // Check ID match
    if (route.src_id != msg.id) continue;
    
    // Rate limiting
    if (route.rate_limit > 0) {
      if (millis() - route.last_forward < route.rate_limit) {
        continue;
      }
      route.last_forward = millis();
    }
    
    // Prepare message for forwarding
    uint32_t forward_id = route.remap_id ? route.dst_id : msg.id;
    
    // Forward the message
    canSend(dst_bus, forward_id, msg.ext, msg.data, msg.len);
    
    // Only forward once per message
    break;
  }
}
