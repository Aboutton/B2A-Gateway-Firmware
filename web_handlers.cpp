#include "web_handlers.h"
#include "web_page.h"
#include "web_monitor.h"
#include <ArduinoJson.h>
#include "config_storage.h"
#include "output_controller.h"

extern WebServer server;
void handleMonitor();
void setupWebServer(WebServer& srv) {
  srv.on("/", handleRoot);
  srv.on("/monitor", handleMonitor);
  srv.on("/api/status", handleStatus);
  srv.on("/api/get_config", handleGetConfig);
  srv.on("/api/set_config", HTTP_POST, handleSetConfig);
  srv.on("/api/save_config", HTTP_POST, handleSaveConfig);
  srv.on("/api/reset_config", HTTP_POST, handleResetConfig);
  
  // Captive portal
  srv.on("/generate_204", handleRoot);
  srv.on("/fwlink", handleRoot);
  srv.on("/hotspot-detect.html", handleRoot);
  srv.onNotFound(handleRoot);
}

void handleRoot() {
  server.send_P(200, "text/html", HTML_PAGE);
}

void handleMonitor() {
  server.send_P(200, "text/html", MONITOR_PAGE);
}

void handleStatus() {
  StaticJsonDocument<512> doc;
  
  doc["uptime"] = state.uptime;
  doc["free_heap"] = state.free_heap;
  doc["can1_tx"] = state.can1_tx_count;
  doc["can1_rx"] = state.can1_rx_count;
  doc["can2_tx"] = state.can2_tx_count;
  doc["can2_rx"] = state.can2_rx_count;
  
  JsonArray aux = doc.createNestedArray("aux");
  for (int i = 0; i < 4; i++) aux.add(state.aux[i]);
  
  JsonArray analog = doc.createNestedArray("analog");
  for (int i = 0; i < 3; i++) analog.add(state.analog_voltage[i]);
  
  doc["temp1"] = state.temp1;
  doc["temp2"] = state.temp2;
  
  JsonArray digital = doc.createNestedArray("digital");
  for (int i = 0; i < 5; i++) digital.add(state.din[i]);
  
  String json;
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

void handleGetConfig() {
  DynamicJsonDocument doc(4096);
  
  // CAN settings
  doc["can1_bitrate"] = gw_config.can1_bitrate;
  doc["can2_bitrate"] = gw_config.can2_bitrate;
  doc["can1_term"] = gw_config.can1_term;
  doc["can2_term"] = gw_config.can2_term;
  
  // AUX triggers
  JsonArray aux = doc.createNestedArray("aux");
  for (int i = 0; i < 4; i++) {
    JsonObject obj = aux.createNestedObject();
    obj["mode"] = gw_config.aux[i].mode;
    obj["can_id"] = gw_config.aux[i].can_id;
    obj["data_byte"] = gw_config.aux[i].data_byte;
    obj["data_value"] = gw_config.aux[i].data_value;
    obj["digital_input"] = gw_config.aux[i].digital_input;
  }
  
  // PWM triggers
  JsonArray pwm = doc.createNestedArray("pwm");
  for (int i = 0; i < 2; i++) {
    JsonObject obj = pwm.createNestedObject();
    obj["mode"] = gw_config.pwm[i].mode;
    obj["can_id"] = gw_config.pwm[i].can_id;
    obj["data_byte"] = gw_config.pwm[i].data_byte;
    obj["scale"] = gw_config.pwm[i].scale;
    obj["frequency"] = gw_config.pwm[i].frequency;
    obj["always_duty"] = gw_config.pwm[i].always_duty;
  }
  
  // Sensor broadcasts
  JsonArray analog_arr = doc.createNestedArray("analog_broadcast");
  for (int i = 0; i < 3; i++) {
    JsonObject obj = analog_arr.createNestedObject();
    obj["enabled"] = gw_config.analog[i].enabled;
    obj["can_id"] = gw_config.analog[i].can_id;
    obj["start_byte"] = gw_config.analog[i].start_byte;
    obj["scale"] = gw_config.analog[i].scale;
  }
  
  // Routes
  JsonArray routes = doc.createNestedArray("routes");
  for (int i = 0; i < gw_config.route_count && i < 40; i++) {
    JsonObject obj = routes.createNestedObject();
    obj["enabled"] = gw_config.routes[i].enabled;
    obj["direction"] = gw_config.routes[i].direction;
    obj["src_id"] = gw_config.routes[i].src_id;
    obj["dst_id"] = gw_config.routes[i].dst_id;
    obj["remap_id"] = gw_config.routes[i].remap_id;
  }
  
  String json;
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

void handleSetConfig() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "No body");
    return;
  }
  
  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  
  if (error) {
    server.send(400, "text/plain", "Invalid JSON");
    return;
  }
  
  // Update configuration from JSON
  if (doc.containsKey("can1_bitrate")) gw_config.can1_bitrate = doc["can1_bitrate"];
  if (doc.containsKey("can2_bitrate")) gw_config.can2_bitrate = doc["can2_bitrate"];
  
  // More parsing would go here...
  
  applyAlwaysOnOutputs();
  server.send(200, "text/plain", "OK");
}

void handleSaveConfig() {
  if (saveConfig()) {
    server.send(200, "text/plain", "Configuration saved");
  } else {
    server.send(500, "text/plain", "Save failed");
  }
}

void handleResetConfig() {
  resetToDefaults();
  applyAlwaysOnOutputs();
  server.send(200, "text/plain", "Reset to defaults");
}
