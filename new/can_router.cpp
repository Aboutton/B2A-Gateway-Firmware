#include "can_router.h"
#include "can_bus.h"

// Pre-built route index for O(1)-ish lookup on exact match
// Maps: route indices split by source bus for fast iteration
static uint8_t bus1Routes[40];
static uint8_t bus1RouteCount = 0;
static uint8_t bus2Routes[40];
static uint8_t bus2RouteCount = 0;
static uint8_t bidiRoutes[40];
static uint8_t bidiRouteCount = 0;

void buildRouteIndex() {
  bus1RouteCount = 0;
  bus2RouteCount = 0;
  bidiRouteCount = 0;

  for (uint8_t i = 0; i < gw_config.route_count && i < 40; i++) {
    if (!gw_config.routes[i].enabled) continue;

    switch (gw_config.routes[i].direction) {
      case ROUTE_CAN1_TO_CAN2:
        bus1Routes[bus1RouteCount++] = i;
        break;
      case ROUTE_CAN2_TO_CAN1:
        bus2Routes[bus2RouteCount++] = i;
        break;
      case ROUTE_BIDIRECTIONAL:
        bidiRoutes[bidiRouteCount++] = i;
        break;
    }
  }

  Serial.printf("Route index built: CAN1->2=%d  CAN2->1=%d  Bidi=%d\n",
    bus1RouteCount, bus2RouteCount, bidiRouteCount);
}

// Check if a CAN message ID matches a route's filter
static inline bool matchesFilter(const CanRoute& route, uint32_t msg_id) {
  switch (route.filter_mode) {
    case FILTER_EXACT:
      return (msg_id == route.src_id);

    case FILTER_MASK:
      return ((msg_id & route.id_mask) == (route.src_id & route.id_mask));

    case FILTER_RANGE:
      return (msg_id >= route.src_id && msg_id <= route.range_end);

    case FILTER_PASS_ALL:
      return true;

    default:
      return false;
  }
}

// Check rate limit — returns true if message should be forwarded
static inline bool checkRateLimit(CanRoute& route) {
  if (route.rate_limit == 0) return true;

  unsigned long now = millis();
  if (now - route.last_forward >= route.rate_limit) {
    route.last_forward = now;
    return true;
  }

  route.drop_count++;
  return false;
}

// Apply data modifications to a copy of the message data
static inline void applyDataModifications(const CanRoute& route, uint8_t* data, uint8_t len) {
  for (uint8_t m = 0; m < 4; m++) {
    const DataModify& mod = route.modifications[m];
    if (!mod.enabled || mod.byte_index >= len) continue;

    switch (mod.operation) {
      case MODIFY_SET:
        // Set specific bits: OR the value (masked)
        data[mod.byte_index] |= (mod.value & mod.bit_mask);
        break;

      case MODIFY_CLEAR:
        // Clear specific bits: AND with inverted mask
        data[mod.byte_index] &= ~(mod.value & mod.bit_mask);
        break;

      case MODIFY_REPLACE:
      default:
        // Replace bits within mask: clear masked bits, then set new value
        data[mod.byte_index] = (data[mod.byte_index] & ~mod.bit_mask) | (mod.value & mod.bit_mask);
        break;
    }
  }
}

// Check if any modifications are enabled for a route
static inline bool hasModifications(const CanRoute& route) {
  for (uint8_t m = 0; m < 4; m++) {
    if (route.modifications[m].enabled) return true;
  }
  return false;
}

// Process a single route against a message
static inline bool processRoute(uint8_t route_idx, uint8_t dst_bus, const CANFDMessage& msg) {
  CanRoute& route = gw_config.routes[route_idx];

  // Filter check
  if (!matchesFilter(route, msg.id)) return false;

  // Rate limit check
  if (!checkRateLimit(route)) return false;

  // Determine forwarded ID
  uint32_t forward_id = route.remap_id ? route.dst_id : msg.id;

  // Apply data modifications if any are enabled
  if (hasModifications(route)) {
    uint8_t modified_data[64];
    uint8_t copy_len = min((uint8_t)msg.len, (uint8_t)64);
    memcpy(modified_data, msg.data, copy_len);
    applyDataModifications(route, modified_data, copy_len);

    if (canSend(dst_bus, forward_id, msg.ext, modified_data, msg.len)) {
      route.forward_count++;
    }
  } else {
    // No modifications — forward original data
    if (canSend(dst_bus, forward_id, msg.ext, msg.data, msg.len)) {
      route.forward_count++;
    }
  }

  // Return whether to continue matching more routes
  return !route.allow_multi_match;  // true = stop, false = keep going
}

void routeMessage(uint8_t src_bus, const CANFDMessage& msg) {
  // Select the right route list based on source bus
  const uint8_t* directRoutes;
  uint8_t directCount;

  if (src_bus == 1) {
    directRoutes = bus1Routes;
    directCount = bus1RouteCount;
  } else {
    directRoutes = bus2Routes;
    directCount = bus2RouteCount;
  }

  uint8_t dst_bus = (src_bus == 1) ? 2 : 1;

  // Check direction-specific routes
  for (uint8_t i = 0; i < directCount; i++) {
    if (processRoute(directRoutes[i], dst_bus, msg)) return;
  }

  // Check bidirectional routes
  for (uint8_t i = 0; i < bidiRouteCount; i++) {
    if (processRoute(bidiRoutes[i], dst_bus, msg)) return;
  }
}

void getRouteStats(uint8_t index, uint32_t& forwarded, uint32_t& dropped) {
  if (index < 40) {
    forwarded = gw_config.routes[index].forward_count;
    dropped = gw_config.routes[index].drop_count;
  } else {
    forwarded = 0;
    dropped = 0;
  }
}

void resetRouteStats() {
  for (uint8_t i = 0; i < 40; i++) {
    gw_config.routes[i].forward_count = 0;
    gw_config.routes[i].drop_count = 0;
  }
}
