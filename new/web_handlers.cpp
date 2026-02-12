#include "web_handlers.h"
#include "web_page.h"
#include "web_monitor.h"
#include <ArduinoJson.h>
#include "config_storage.h"
#include "output_controller.h"
#include "pwm_control.h"

extern WebServer server;

static const char REDIRECT_URL[] = "http://b2a-gateway.local";

// Redirect to mDNS hostname
void handleCaptiveRedirect() {
  server.sendHeader("Location", REDIRECT_URL, true);
  server.send(302, "text/html",
    "<html><body><a href=\"http://b2a-gateway.local\">Click here</a></body></html>");
}

// For Android: must return HTTP 204 for detection, then redirect
void handleGenerate204() {
  server.sendHeader("Location", REDIRECT_URL, true);
  server.send(302, "text/plain", "");
}

// For Apple: must NOT return 200 with "Success" to trigger portal popup
void handleAppleCaptive() {
  server.sendHeader("Location", REDIRECT_URL, true);
  server.send(302, "text/html",
    "<html><body><a href=\"http://b2a-gateway.local\">Click here</a></body></html>");
}

// Check if request is for our own hostname — if so, serve normally; otherwise redirect
void handleNotFound() {
  String host = server.hostHeader();
  if (host == "b2a-gateway.local" || host == "192.168.4.1") {
    // Serve the main page for our own hostname
    server.send_P(200, "text/html", HTML_PAGE);
  } else {
    // Any other hostname = captive portal redirect
    handleCaptiveRedirect();
  }
}

void handleMonitor();
void setupWebServer(WebServer& srv) {
  srv.on("/", handleRoot);
  srv.on("/monitor", handleMonitor);
  srv.on("/api/status", handleStatus);
  srv.on("/api/get_config", handleGetConfig);
  srv.on("/api/set_config", HTTP_POST, handleSetConfig);
  srv.on("/api/save_config", HTTP_POST, handleSaveConfig);
  srv.on("/api/reset_config", HTTP_POST, handleResetConfig);

  // Captive portal detection endpoints (all major OS)
  // Android
  srv.on("/generate_204", handleGenerate204);
  srv.on("/gen_204", handleGenerate204);
  srv.on("/connecttest.txt", handleCaptiveRedirect);
  // Apple iOS / macOS
  srv.on("/hotspot-detect.html", handleAppleCaptive);
  srv.on("/library/test/success.html", handleAppleCaptive);
  // Windows
  srv.on("/fwlink", handleCaptiveRedirect);
  srv.on("/connecttest.txt", handleCaptiveRedirect);
  srv.on("/redirect", handleCaptiveRedirect);
  srv.on("/ncsi.txt", handleCaptiveRedirect);
  // Firefox
  srv.on("/canonical.html", handleCaptiveRedirect);
  srv.on("/success.txt", handleCaptiveRedirect);

  // Any unknown URL — check host and redirect if not our hostname
  srv.onNotFound(handleNotFound);
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
  doc["pcf_available"] = state.pcf_available;
  doc["ads_available"] = state.ads_available;
  doc["can1_tx"] = state.can1_tx_count;
  doc["can1_rx"] = state.can1_rx_count;
  doc["can2_tx"] = state.can2_tx_count;
  doc["can2_rx"] = state.can2_rx_count;

  JsonArray aux = doc.createNestedArray("aux");
  for (int i = 0; i < 4; i++) aux.add(state.aux[i]);

  JsonArray pwm = doc.createNestedArray("pwm");
  pwm.add(config.pwm1_duty);
  pwm.add(config.pwm2_duty);

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
  StaticJsonDocument<4096> doc;

  // CAN settings
  doc["can1_bitrate"] = gw_config.can1_bitrate;
  doc["can2_bitrate"] = gw_config.can2_bitrate;
  doc["can1_term"] = gw_config.can1_term;
  doc["can2_term"] = gw_config.can2_term;
  doc["sensor_interval"] = gw_config.sensor_interval;

  // AUX triggers
  JsonArray aux = doc.createNestedArray("aux");
  for (int i = 0; i < 4; i++) {
    JsonObject obj = aux.createNestedObject();
    obj["mode"] = gw_config.aux[i].mode;
    obj["can_id"] = gw_config.aux[i].can_id;
    obj["data_byte"] = gw_config.aux[i].data_byte;
    obj["data_value"] = gw_config.aux[i].data_value;
    obj["data_mask"] = gw_config.aux[i].data_mask;
    obj["digital_input"] = gw_config.aux[i].digital_input;
    obj["invert"] = gw_config.aux[i].invert;
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
    obj["digital_input"] = gw_config.pwm[i].digital_input;
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
    obj["offset"] = gw_config.analog[i].offset;
  }

  // Temperature broadcasts
  JsonArray temp_arr = doc.createNestedArray("temp_broadcast");
  for (int i = 0; i < 2; i++) {
    JsonObject obj = temp_arr.createNestedObject();
    obj["enabled"] = gw_config.temp[i].enabled;
    obj["can_id"] = gw_config.temp[i].can_id;
    obj["start_byte"] = gw_config.temp[i].start_byte;
    obj["scale"] = gw_config.temp[i].scale;
    obj["offset"] = gw_config.temp[i].offset;
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
    obj["rate_limit"] = gw_config.routes[i].rate_limit;
  }

  String json;
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

// Valid CAN bitrates
static bool isValidBitrate(uint32_t br) {
  const uint32_t valid[] = {125000, 250000, 500000, 800000, 1000000};
  for (uint8_t i = 0; i < 5; i++) {
    if (br == valid[i]) return true;
  }
  return false;
}

void handleSetConfig() {
  Serial.println("\n=== handleSetConfig called ===");

  if (!server.hasArg("plain")) {
    Serial.println("ERROR: No body received");
    server.send(400, "text/plain", "No body");
    return;
  }

  Serial.println("Body received:");
  Serial.println(server.arg("plain"));

  StaticJsonDocument<4096> doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));

  if (error) {
    Serial.print("ERROR: JSON parse failed: ");
    Serial.println(error.c_str());
    server.send(400, "text/plain", "Invalid JSON");
    return;
  }

  Serial.println("JSON parsed successfully");

  // CAN settings with validation
  if (doc.containsKey("can1_bitrate")) {
    uint32_t br = doc["can1_bitrate"];
    if (isValidBitrate(br)) {
      gw_config.can1_bitrate = br;
      Serial.printf("  can1_bitrate = %d\n", gw_config.can1_bitrate);
    } else {
      Serial.printf("  can1_bitrate rejected: %d\n", br);
    }
  }
  if (doc.containsKey("can2_bitrate")) {
    uint32_t br = doc["can2_bitrate"];
    if (isValidBitrate(br)) {
      gw_config.can2_bitrate = br;
      Serial.printf("  can2_bitrate = %d\n", gw_config.can2_bitrate);
    } else {
      Serial.printf("  can2_bitrate rejected: %d\n", br);
    }
  }
  if (doc.containsKey("can1_term")) {
    gw_config.can1_term = doc["can1_term"];
    Serial.printf("  can1_term = %s\n", gw_config.can1_term ? "true" : "false");
  }
  if (doc.containsKey("can2_term")) {
    gw_config.can2_term = doc["can2_term"];
    Serial.printf("  can2_term = %s\n", gw_config.can2_term ? "true" : "false");
  }
  if (doc.containsKey("sensor_interval")) {
    uint16_t interval = doc["sensor_interval"];
    if (interval >= 10 && interval <= 10000) {
      gw_config.sensor_interval = interval;
    }
  }

  // AUX triggers with validation
  if (doc.containsKey("aux")) {
    Serial.println("  Updating AUX triggers...");
    JsonArray aux = doc["aux"];
    for (int i = 0; i < 4 && i < (int)aux.size(); i++) {
      uint8_t mode = aux[i]["mode"] | 0;
      if (mode <= TRIGGER_ALWAYS_ON) {
        gw_config.aux[i].mode = mode;
      }
      gw_config.aux[i].can_id = aux[i]["can_id"] | (uint32_t)0;
      uint8_t db = aux[i]["data_byte"] | 0;
      if (db < 8) gw_config.aux[i].data_byte = db;
      gw_config.aux[i].data_value = aux[i]["data_value"] | 0;
      gw_config.aux[i].data_mask = aux[i]["data_mask"] | 0xFF;
      uint8_t din = aux[i]["digital_input"] | 0;
      if (din <= 5) gw_config.aux[i].digital_input = din;
      gw_config.aux[i].invert = aux[i]["invert"] | false;
      Serial.printf("    AUX%d: mode=%d, id=0x%X\n", i+1, gw_config.aux[i].mode, gw_config.aux[i].can_id);
    }
  }

  // PWM triggers with validation
  if (doc.containsKey("pwm")) {
    Serial.println("  Updating PWM triggers...");
    JsonArray pwm = doc["pwm"];
    for (int i = 0; i < 2 && i < (int)pwm.size(); i++) {
      uint8_t mode = pwm[i]["mode"] | 0;
      if (mode <= TRIGGER_ALWAYS_ON) {
        gw_config.pwm[i].mode = mode;
      }
      gw_config.pwm[i].can_id = pwm[i]["can_id"] | (uint32_t)0;
      uint8_t db = pwm[i]["data_byte"] | 0;
      if (db < 8) gw_config.pwm[i].data_byte = db;
      float scale = pwm[i]["scale"] | 1.0f;
      if (scale > 0 && scale <= 100.0) gw_config.pwm[i].scale = scale;
      uint32_t freq = pwm[i]["frequency"] | 1000;
      if (freq >= 1 && freq <= 40000) gw_config.pwm[i].frequency = freq;
      uint8_t din = pwm[i]["digital_input"] | 0;
      if (din <= 5) gw_config.pwm[i].digital_input = din;
      uint8_t duty = pwm[i]["always_duty"] | 0;
      if (duty <= 100) gw_config.pwm[i].always_duty = duty;
      Serial.printf("    PWM%d: mode=%d, freq=%d\n", i+1, gw_config.pwm[i].mode, gw_config.pwm[i].frequency);
    }
  }

  // Sensor broadcasts with validation
  if (doc.containsKey("analog_broadcast")) {
    Serial.println("  Updating analog broadcasts...");
    JsonArray analog_arr = doc["analog_broadcast"];
    for (int i = 0; i < 3 && i < (int)analog_arr.size(); i++) {
      gw_config.analog[i].enabled = analog_arr[i]["enabled"] | false;
      gw_config.analog[i].can_id = analog_arr[i]["can_id"] | (uint32_t)0;
      uint8_t sb = analog_arr[i]["start_byte"] | 0;
      if (sb <= 6) gw_config.analog[i].start_byte = sb;
      gw_config.analog[i].scale = analog_arr[i]["scale"] | 1.0f;
      gw_config.analog[i].offset = analog_arr[i]["offset"] | 0.0f;
    }
  }

  // Temperature broadcasts with validation
  if (doc.containsKey("temp_broadcast")) {
    Serial.println("  Updating temp broadcasts...");
    JsonArray temp_arr = doc["temp_broadcast"];
    for (int i = 0; i < 2 && i < (int)temp_arr.size(); i++) {
      gw_config.temp[i].enabled = temp_arr[i]["enabled"] | false;
      gw_config.temp[i].can_id = temp_arr[i]["can_id"] | (uint32_t)0;
      uint8_t sb = temp_arr[i]["start_byte"] | 0;
      if (sb <= 6) gw_config.temp[i].start_byte = sb;
      gw_config.temp[i].scale = temp_arr[i]["scale"] | 1.0f;
      gw_config.temp[i].offset = temp_arr[i]["offset"] | 0.0f;
    }
  }

  // Routes with validation
  if (doc.containsKey("routes")) {
    Serial.println("  Updating routes...");
    JsonArray routes = doc["routes"];
    gw_config.route_count = min((int)routes.size(), 40);
    for (int i = 0; i < gw_config.route_count; i++) {
      gw_config.routes[i].enabled = routes[i]["enabled"] | false;
      uint8_t dir = routes[i]["direction"] | 0;
      if (dir <= ROUTE_BIDIRECTIONAL) gw_config.routes[i].direction = dir;
      gw_config.routes[i].src_id = routes[i]["src_id"] | (uint32_t)0;
      gw_config.routes[i].dst_id = routes[i]["dst_id"] | (uint32_t)0;
      gw_config.routes[i].remap_id = routes[i]["remap_id"] | false;
      gw_config.routes[i].rate_limit = routes[i]["rate_limit"] | 0;
      gw_config.routes[i].last_forward = 0;
    }
  }

  Serial.println("Applying always-on outputs...");
  applyAlwaysOnOutputs();

  Serial.println("=== handleSetConfig complete ===\n");
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
