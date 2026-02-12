#include "output_controller.h"
#include "io_control.h"
#include "pwm_control.h"

void initOutputController() {
  applyAlwaysOnOutputs();
  Serial.println("✓ Output controller initialized");
}

void processOutputTriggers(uint8_t bus, const CANFDMessage& msg) {
  // Check AUX triggers
  for (uint8_t i = 0; i < 4; i++) {
    AuxTrigger& trigger = gw_config.aux[i];
    
    if (trigger.mode == TRIGGER_CAN && trigger.can_id == msg.id) {
      if (trigger.data_byte < msg.len) {
        uint8_t data = msg.data[trigger.data_byte];
        uint8_t masked = data & trigger.data_mask;
        bool match = (masked == trigger.data_value);
        
        if (trigger.invert) match = !match;
        
        setAux(i + 1, match);
      }
    }
  }
  
  // Check PWM triggers
  for (uint8_t i = 0; i < 2; i++) {
    PwmTrigger& trigger = gw_config.pwm[i];
    
    if (trigger.mode == TRIGGER_CAN && trigger.can_id == msg.id) {
      if (trigger.data_byte < msg.len) {
        uint8_t data = msg.data[trigger.data_byte];
        uint8_t duty = constrain(data * trigger.scale, 0, 100);
        setPWM(i + 1, duty, trigger.frequency);
      }
    }
  }
}

void updateOutputsFromDigitalInputs() {
  // Check AUX DIN triggers
  for (uint8_t i = 0; i < 4; i++) {
    AuxTrigger& trigger = gw_config.aux[i];
    
    if (trigger.mode == TRIGGER_DIN && trigger.digital_input >= 1 && trigger.digital_input <= 5) {
      bool din_state = state.din[trigger.digital_input - 1];
      if (trigger.invert) din_state = !din_state;
      setAux(i + 1, din_state);
    }
  }
  
  // Check PWM DIN triggers
  for (uint8_t i = 0; i < 2; i++) {
    PwmTrigger& trigger = gw_config.pwm[i];
    
    if (trigger.mode == TRIGGER_DIN && trigger.digital_input >= 1 && trigger.digital_input <= 5) {
      bool din_state = state.din[trigger.digital_input - 1];
      uint8_t duty = din_state ? trigger.always_duty : 0;
      setPWM(i + 1, duty, trigger.frequency);
    }
  }
}

void applyAlwaysOnOutputs() {
  // Apply Always ON triggers
  for (uint8_t i = 0; i < 4; i++) {
    if (gw_config.aux[i].mode == TRIGGER_ALWAYS_ON) {
      setAux(i + 1, true);
    } else if (gw_config.aux[i].mode == TRIGGER_OFF) {
      setAux(i + 1, false);
    }
  }
  
  for (uint8_t i = 0; i < 2; i++) {
    if (gw_config.pwm[i].mode == TRIGGER_ALWAYS_ON) {
      setPWM(i + 1, gw_config.pwm[i].always_duty, gw_config.pwm[i].frequency);
    } else if (gw_config.pwm[i].mode == TRIGGER_OFF) {
      setPWM(i + 1, 0, gw_config.pwm[i].frequency);
    }
  }
}
