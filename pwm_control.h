#ifndef PWM_CONTROL_H
#define PWM_CONTROL_H

#include <Arduino.h>
#include "config.h"

void setupPWM();
void setPWM(uint8_t channel, uint8_t duty, uint32_t freq);

#endif
