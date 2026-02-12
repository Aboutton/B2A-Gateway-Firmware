#include "pwm_control.h"

static uint32_t currentFreq1 = 0;
static uint32_t currentFreq2 = 0;

void setupPWM() {
  pinMode(PWM1_PIN, OUTPUT);
  pinMode(PWM2_PIN, OUTPUT);
  setPWM(1, 0, 1000);
  setPWM(2, 0, 1000);
}

void setPWM(uint8_t channel, uint8_t duty, uint32_t freq) {
  if (channel == 1) {
    config.pwm1_duty = duty;
    config.pwm1_freq = freq;
    if (freq != currentFreq1) {
      ledcAttach(PWM1_PIN, freq, 8);
      currentFreq1 = freq;
    }
    ledcWrite(PWM1_PIN, duty * 255 / 100);
  } else if (channel == 2) {
    config.pwm2_duty = duty;
    config.pwm2_freq = freq;
    if (freq != currentFreq2) {
      ledcAttach(PWM2_PIN, freq, 8);
      currentFreq2 = freq;
    }
    ledcWrite(PWM2_PIN, duty * 255 / 100);
  }
}
