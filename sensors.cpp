#include "sensors.h"
#include "io_control.h"

Adafruit_ADS1115 ads;

void setupTemperatureSensors() {
  pinMode(TEMP1_PIN, INPUT);
  pinMode(TEMP2_PIN, INPUT);
  pinMode(TEMP1_PULLUP, OUTPUT);
  pinMode(TEMP2_PULLUP, OUTPUT);
}

float readNTCTemperature(uint8_t pin) {
  int raw = analogRead(pin);
  float voltage = (raw / 4095.0) * 3.3;
  float resistance = NTC_SERIES_RESISTOR * voltage / (3.3 - voltage);
  
  // Steinhart-Hart equation
  float steinhart = resistance / NTC_NOMINAL_RESISTANCE;
  steinhart = log(steinhart);
  steinhart /= NTC_BETA;
  steinhart += 1.0 / (NTC_NOMINAL_TEMP + 273.15);
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15;
  
  return steinhart;
}

void setupADC() {
  state.ads_available = ads.begin(ADS1115_ADDR);
  if (state.ads_available) {
    ads.setGain(GAIN_ONE);
  }
}

void readTemperatures() {
  digitalWrite(TEMP1_PULLUP, HIGH);
  digitalWrite(TEMP2_PULLUP, HIGH);
  delay(10);
  
  state.temp1 = readNTCTemperature(TEMP1_PIN);
  state.temp2 = readNTCTemperature(TEMP2_PIN);
}

void readAnalogInputs() {
  if (!state.ads_available) return;
  
  for (int i = 0; i < 3; i++) {
    int16_t raw = ads.readADC_SingleEnded(i);
    state.analog_voltage[i] = raw * ADC_VOLTAGE_SCALE;
  }
}

void updateSensors() {
  readTemperatures();
  readAnalogInputs();
  readAllDigitalInputs();
  
  state.uptime = millis() / 1000;
  state.free_heap = ESP.getFreeHeap();
}
