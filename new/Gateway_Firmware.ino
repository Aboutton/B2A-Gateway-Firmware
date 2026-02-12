/*
 * B2A CAN Gateway - Production Firmware
 * ESP32-S3-WROOM-1-N16R8
 *
 * Features:
 * - Bidirectional CAN routing (CAN1 <-> CAN2) with speed conversion
 * - Output control (AUX, PWM) via CAN, Digital Input, or Always ON
 * - Sensor broadcasting (Analog, Temp) to CAN
 * - WiFi configuration interface
 * - Bluetooth monitoring
 * - Flash-based configuration storage
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ESPmDNS.h>
#include <DNSServer.h>

#include "config.h"
#include "gateway_config.h"
#include "config_storage.h"
#include "io_control.h"
#include "sensors.h"
#include "can_bus.h"
#include "pwm_control.h"
#include "output_controller.h"
#include "sensor_broadcaster.h"
#include "can_router.h"
#include "web_handlers.h"
#include "websocket_handler.h"
// #include "bluetooth_handler.h"

// Global instances
Config config;
State state;
GatewayConfig gw_config;
String wifi_ssid;

WebServer server(80);
WebSocketsServer webSocket(81);
DNSServer dnsServer;

// Timing
unsigned long lastUpdate = 0;
unsigned long lastSensorBroadcast = 0;

String getChipID() {
  uint64_t chipid = ESP.getEfuseMac();
  char id[13];
  sprintf(id, "%04X%08X", (uint16_t)(chipid >> 32), (uint32_t)chipid);
  return String(id);
}

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println("\n\n=====================================");
  Serial.println("B2A CAN Gateway - Production Mode");
  Serial.println("=====================================\n");

  // Get chip ID
  String chipID = getChipID();
  wifi_ssid = "B2A-GW-" + chipID;
  Serial.println("Chip ID: " + chipID);

  // Initialize flash storage and load config
  initConfigStorage();
  loadConfig();

  // Initialize hardware modules
  setupPCF8575();
  setupADC();
  setupTemperatureSensors();
  setupPWM();
  setupCAN();

  // Initialize gateway modules
  initOutputController();
  buildRouteIndex();

  // Start WiFi if enabled
  if (gw_config.wifi_enabled) {
    Serial.println("\nStarting WiFi AP...");
    WiFi.softAP(wifi_ssid.c_str(), gw_config.wifi_password);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP: ");
    Serial.println(IP);

    // Start DNS server - redirect ALL DNS queries to our IP (captive portal)
    dnsServer.start(53, "*", IP);
    Serial.println("DNS captive portal started");

    // Start mDNS
    if (MDNS.begin("b2a-gateway")) {
      Serial.println("mDNS started: http://b2a-gateway.local");
    }

    // Start web server
    setupWebServer(server);
    server.begin();
    Serial.println("Web server started on port 80");

    // Start WebSocket
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    Serial.println("WebSocket started on port 81");
  }

  // Start Bluetooth if enabled
  // if (gw_config.bluetooth_enabled) {
  //   initBluetooth();
  // }

  Serial.println("\n=====================================");
  Serial.println("Gateway Ready!");
  Serial.println("WiFi SSID: " + wifi_ssid);
  Serial.println("Bluetooth: " + String(gw_config.bt_name));
  Serial.println("=====================================\n");
}

void loop() {
  // Handle communication
  if (gw_config.wifi_enabled) {
    dnsServer.processNextRequest();
    server.handleClient();
    webSocket.loop();
  }

  // if (gw_config.bluetooth_enabled) {
  //   handleBluetoothData();
  // }

  // Poll CAN buses and process messages
  CANFDMessage msg;

  if (gw_config.can1_enabled) {
    while (can1.receive(msg)) {
      state.can1_rx_count++;

      // Process for output triggers
      processOutputTriggers(1, msg);

      // Route to CAN2 if rules match
      routeMessage(1, msg);
    }
  }

  if (gw_config.can2_enabled) {
    while (can2.receive(msg)) {
      state.can2_rx_count++;

      // Route to CAN1 if rules match
      routeMessage(2, msg);
    }
  }

  // Update sensors and outputs periodically
  if (millis() - lastUpdate >= 500) {
    lastUpdate = millis();
    updateSensors();
    updateOutputsFromDigitalInputs();

    // Send WebSocket status update
    if (gw_config.wifi_enabled) {
      sendWebSocketStatusUpdate(webSocket, state, config);
    }
  }

  // Broadcast sensor data on CAN
  if (millis() - lastSensorBroadcast >= gw_config.sensor_interval) {
    lastSensorBroadcast = millis();
    broadcastSensors();
  }

  // Send Bluetooth status
  // if (gw_config.bluetooth_enabled) {
  //   sendBluetoothStatus();
  // }

  delay(1);
}
