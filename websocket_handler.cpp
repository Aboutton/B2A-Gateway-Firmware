#include "websocket_handler.h"
#include <ArduinoJson.h>

extern WebSocketsServer webSocket;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] Received: %s\n", num, payload);
      break;
  }
}

void sendWebSocketStatusUpdate(WebSocketsServer& ws, const State& st, const Config& cfg) {
  StaticJsonDocument<768> doc;
  doc["type"] = "status_update";
  
  doc["uptime"] = st.uptime;
  doc["free_heap"] = st.free_heap;
  doc["pcf_available"] = st.pcf_available;
  doc["ads_available"] = st.ads_available;
  
  doc["can1_tx"] = st.can1_tx_count;
  doc["can1_rx"] = st.can1_rx_count;
  doc["can2_tx"] = st.can2_tx_count;
  doc["can2_rx"] = st.can2_rx_count;
  
  JsonArray aux = doc.createNestedArray("aux");
  for (int i = 0; i < 4; i++) {
    aux.add(st.aux[i]);
  }
  
  JsonArray pwm = doc.createNestedArray("pwm");
  pwm.add(cfg.pwm1_duty);
  pwm.add(cfg.pwm2_duty);
  
  JsonArray analog = doc.createNestedArray("analog");
  for (int i = 0; i < 3; i++) {
    analog.add(st.analog_voltage[i]);
  }
  
  JsonArray digital = doc.createNestedArray("digital");
  for (int i = 0; i < 5; i++) {
    digital.add(st.din[i]);
  }
  
  doc["temp1"] = st.temp1;
  doc["temp2"] = st.temp2;
  
  String json;
  serializeJson(doc, json);
  ws.broadcastTXT(json);
}