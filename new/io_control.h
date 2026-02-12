#ifndef IO_CONTROL_H
#define IO_CONTROL_H

#include <Arduino.h>
#include <Wire.h>
#include "config.h"

// PCF8575 functions
bool pcf8575_write(uint16_t value);
uint16_t pcf8575_read();
void pcf8575_setBit(uint8_t bit, bool value);

// AUX output control
void setAux(uint8_t aux, bool state_val);

// LED control
void setLED(uint8_t led, bool state_val);

// Digital input reading
bool readDigitalInput(uint8_t din_num);
void readAllDigitalInputs();

// Initialize PCF8575
void setupPCF8575();

#endif
