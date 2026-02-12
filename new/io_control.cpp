#include "io_control.h"

bool pcf8575_write(uint16_t value) {
  Wire.beginTransmission(PCF8575_ADDR);
  Wire.write(value & 0xFF);
  Wire.write((value >> 8) & 0xFF);
  return (Wire.endTransmission() == 0);
}

uint16_t pcf8575_read() {
  Wire.requestFrom(PCF8575_ADDR, 2);
  if (Wire.available() >= 2) {
    uint8_t low = Wire.read();
    uint8_t high = Wire.read();
    return (high << 8) | low;
  }
  return 0;
}

void pcf8575_setBit(uint8_t bit, bool value) {
  if (value) {
    state.pcf_state |= (1 << bit);
  } else {
    state.pcf_state &= ~(1 << bit);
  }
  pcf8575_write(state.pcf_state);
}

void setAux(uint8_t aux, bool state_val) {
  if (aux >= 1 && aux <= 4) {
    state.aux[aux - 1] = state_val;
    pcf8575_setBit(PCF_AUX1 + aux - 1, state_val);
  }
}

void setLED(uint8_t led, bool state_val) {
  if (led >= 1 && led <= 6) {
    state.led[led - 1] = state_val;
    // LEDs are active-LOW: 0=ON, 1=OFF
    pcf8575_setBit(PCF_LED1 + led - 1, !state_val);
  }
}

bool readDigitalInput(uint8_t din_num) {
  if (din_num < 1 || din_num > 5) return false;
  
  uint16_t data = pcf8575_read();
  uint8_t bit = PCF_DIN1 + (din_num - 1);
  
  return (data >> bit) & 1;
}

void readAllDigitalInputs() {
  if (!state.pcf_available) return;
  
  uint16_t pcf_read = pcf8575_read();
  state.din[0] = (pcf_read >> PCF_DIN1) & 1;
  state.din[1] = (pcf_read >> PCF_DIN2) & 1;
  state.din[2] = (pcf_read >> PCF_DIN3) & 1;
  state.din[3] = (pcf_read >> PCF_DIN4) & 1;
  state.din[4] = (pcf_read >> PCF_DIN5) & 1;
}

void setupPCF8575() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.beginTransmission(PCF8575_ADDR);
  state.pcf_available = (Wire.endTransmission() == 0);
  
  if (state.pcf_available) {
    // AUX outputs LOW (bits 0-3 = 0)
    // Digital inputs HIGH to enable reading (bits 5-9 = 1)
    // LEDs OFF = HIGH (bits 10-15 = 1)
    state.pcf_state = 0xFFE0; // Binary: 1111111111100000
    pcf8575_write(state.pcf_state);
    Serial.println("✓ PCF8575 initialized - AUX outputs LOW");
  }
}