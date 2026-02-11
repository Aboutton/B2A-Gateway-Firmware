#include "can_bus.h"
#include <ArduinoJson.h>
#include <WebSocketsServer.h>

extern WebSocketsServer webSocket;

ACAN2517FD can1(CAN1_CS, SPI, 255);
ACAN2517FD can2(CAN2_CS, SPI, 255);

void setupCAN() {
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  pinMode(CAN1_CS, OUTPUT);
  digitalWrite(CAN1_CS, HIGH);
  pinMode(CAN2_CS, OUTPUT);
  digitalWrite(CAN2_CS, HIGH);
  pinMode(CAN1_TERM, OUTPUT);
  pinMode(CAN2_TERM, OUTPUT);
  
  if (config.can1_enabled) {
    ACAN2517FDSettings settings(ACAN2517FDSettings::OSC_40MHz, config.can_bitrate, DataBitRateFactor::x1);
    settings.mRequestedMode = ACAN2517FDSettings::Normal20B;
    
    // Accept all CAN messages
    ACAN2517FDFilters filters;
    filters.appendPassAllFilter(nullptr);
    uint32_t errorCode = can1.begin(settings, nullptr, filters);
    
    if (errorCode == 0) {
      configureMCP_GPIO0(CAN1_CS);
      Serial.println("CAN1 initialized");
      Serial.printf("  Bitrate: %d bps\n", config.can_bitrate);
      Serial.println("  RX enabled - waiting for messages...");
    } else {
      Serial.printf("CAN1 init FAILED: error code 0x%X\n", errorCode);
    }
  }
  
  if (config.can2_enabled) {
    ACAN2517FDSettings settings(ACAN2517FDSettings::OSC_40MHz, config.can_bitrate, DataBitRateFactor::x1);
    settings.mRequestedMode = ACAN2517FDSettings::Normal20B;
    
    ACAN2517FDFilters filters;
    filters.appendPassAllFilter(nullptr);
    uint32_t errorCode = can2.begin(settings, nullptr, filters);
    
    if (errorCode == 0) {
      configureMCP_GPIO0(CAN2_CS);
      Serial.println("CAN2 initialized");
      Serial.printf("  Bitrate: %d bps\n", config.can_bitrate);
      Serial.println("  RX enabled - waiting for messages...");
    } else {
      Serial.printf("CAN2 init FAILED: error code 0x%X\n", errorCode);
    }
  }
  
  digitalWrite(CAN1_TERM, config.can1_term ? HIGH : LOW);
  digitalWrite(CAN2_TERM, config.can2_term ? HIGH : LOW);
  
  Serial.printf("\nðŸ“¡ CAN Termination: CAN1=%s CAN2=%s\n", 
    config.can1_term ? "ON" : "OFF",
    config.can2_term ? "ON" : "OFF");
  Serial.println("Send CAN message from PeakCAN to test RX...\n");
}

void configureMCP_GPIO0(uint8_t cs) {
  SPISettings spi_settings(8000000, MSBFIRST, SPI_MODE0);
  
  SPI.beginTransaction(spi_settings);
  digitalWrite(cs, LOW);
  delayMicroseconds(1);
  SPI.transfer(0x2E);
  SPI.transfer(0x04);
  SPI.transfer(0x40);
  digitalWrite(cs, HIGH);
  SPI.endTransaction();
  delayMicroseconds(50);
  
  SPI.beginTransaction(spi_settings);
  digitalWrite(cs, LOW);
  delayMicroseconds(1);
  SPI.transfer(0x2E);
  SPI.transfer(0x05);
  SPI.transfer(0x00);
  digitalWrite(cs, HIGH);
  SPI.endTransaction();
  delayMicroseconds(50);
  
  SPI.beginTransaction(spi_settings);
  digitalWrite(cs, LOW);
  delayMicroseconds(1);
  SPI.transfer(0x2E);
  SPI.transfer(0x07);
  SPI.transfer(0x01);
  digitalWrite(cs, HIGH);
  SPI.endTransaction();
}

bool canSend(uint8_t bus, uint32_t id, bool ext, const uint8_t* data, uint8_t dlc) {
  CANFDMessage msg;
  msg.type = CANFDMessage::CAN_DATA;
  msg.ext = ext;
  msg.id = ext ? (id & 0x1FFFFFFF) : (id & 0x7FF);
  msg.len = (dlc > 8) ? 8 : dlc;
  
  for (uint8_t i = 0; i < msg.len; i++) {
    msg.data[i] = data[i];
  }
  
  bool success = false;
  if (bus == 1 && config.can1_enabled) {
    success = can1.tryToSend(msg);
    if (success) state.can1_tx_count++;
  } else if (bus == 2 && config.can2_enabled) {
    success = can2.tryToSend(msg);
    if (success) state.can2_tx_count++;
  }
  
  return success;
}

void pollCAN() {
  CANFDMessage msg;
  static unsigned long lastDebug = 0;
  static uint32_t lastRxCount1 = 0;
  static uint32_t lastRxCount2 = 0;
  
  // Debug output every 5 seconds
  if (millis() - lastDebug > 5000) {
    lastDebug = millis();
    if (config.can1_enabled) {
      Serial.printf("CAN1: TX=%d RX=%d (new=%d)\n", 
        state.can1_tx_count, 
        state.can1_rx_count,
        state.can1_rx_count - lastRxCount1);
      lastRxCount1 = state.can1_rx_count;
    }
    if (config.can2_enabled) {
      Serial.printf("CAN2: TX=%d RX=%d (new=%d)\n", 
        state.can2_tx_count, 
        state.can2_rx_count,
        state.can2_rx_count - lastRxCount2);
      lastRxCount2 = state.can2_rx_count;
    }
  }
  
  if (config.can1_enabled) {
    while (can1.receive(msg)) {
      state.can1_rx_count++;
      
      Serial.printf("CAN1 RX: ID=0x%X DLC=%d Data:", msg.id, msg.len);
      for (uint8_t i = 0; i < msg.len; i++) {
        Serial.printf(" %02X", msg.data[i]);
      }
      Serial.println();
      
      StaticJsonDocument<256> doc;
      doc["type"] = "can_rx";
      doc["bus"] = 1;
      doc["id"] = msg.id;
      doc["ext"] = msg.ext;
      doc["dlc"] = msg.len;
      JsonArray data = doc.createNestedArray("data");
      for (uint8_t i = 0; i < msg.len; i++) {
        data.add(msg.data[i]);
      }
      doc["timestamp"] = millis();
      
      String json;
      serializeJson(doc, json);
      webSocket.broadcastTXT(json);
    }
  }
  
  if (config.can2_enabled) {
    while (can2.receive(msg)) {
      state.can2_rx_count++;
      
      Serial.printf("CAN2 RX: ID=0x%X DLC=%d Data:", msg.id, msg.len);
      for (uint8_t i = 0; i < msg.len; i++) {
        Serial.printf(" %02X", msg.data[i]);
      }
      Serial.println();
      
      StaticJsonDocument<256> doc;
      doc["type"] = "can_rx";
      doc["bus"] = 2;
      doc["id"] = msg.id;
      doc["ext"] = msg.ext;
      doc["dlc"] = msg.len;
      JsonArray data = doc.createNestedArray("data");
      for (uint8_t i = 0; i < msg.len; i++) {
        data.add(msg.data[i]);
      }
      doc["timestamp"] = millis();
      
      String json;
      serializeJson(doc, json);
      webSocket.broadcastTXT(json);
    }
  }
}