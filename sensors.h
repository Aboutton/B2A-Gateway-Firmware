#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <Adafruit_ADS1X15.h>
#include "config.h"

// Global ADC object
extern Adafruit_ADS1115 ads;

// Temperature sensor functions
float readNTCTemperature(uint8_t pin);
void setupTemperatureSensors();
void readTemperatures();

// Analog input functions
void setupADC();
void readAnalogInputs();

// Combined sensor update
void updateSensors();

#endif
