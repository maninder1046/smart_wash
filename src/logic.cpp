#include "logic.h"
#include "washserver.h"
#include <Arduino.h>

// Pin assignments
#define WASH_CW_PIN    2
#define WASH_CCW_PIN   15
#define SPIN_PIN       4
#define INLET_PIN      5
#define DRAIN_PIN      18
#define DRYER_PIN      19
#define DRY_RUN_PIN    12
#define WATER_LEVEL_PIN 14

// State timing
unsigned long stateStartTime = 0;
const unsigned long WASH_TIME = 20000;   // 20s
const unsigned long SPIN_TIME = 15000;   // 15s

// Manual control flags
bool manualWash  = false;
bool manualSpin  = false;
bool manualFill  = false;
bool manualDrain = false;
bool manualDryer = false;

// Current states
WasherState washerState = W_IDLE;
DryerState dryerState   = D_IDLE;

void allOff() {
  digitalWrite(WASH_CW_PIN, LOW);
  digitalWrite(WASH_CCW_PIN, LOW);
  digitalWrite(SPIN_PIN, LOW);
  digitalWrite(INLET_PIN, LOW);
  digitalWrite(DRAIN_PIN, LOW);
  digitalWrite(DRYER_PIN, LOW);
}

void initLogic() {
  pinMode(WASH_CW_PIN, OUTPUT);
  pinMode(WASH_CCW_PIN, OUTPUT);
  pinMode(SPIN_PIN, OUTPUT);
  pinMode(INLET_PIN, OUTPUT);
  pinMode(DRAIN_PIN, OUTPUT);
  pinMode(DRYER_PIN, OUTPUT);
  pinMode(DRY_RUN_PIN, INPUT_PULLUP);
  pinMode(WATER_LEVEL_PIN, INPUT_PULLUP);

  allOff();
  stateStartTime = millis();
}

void runLogic() {
  // Manual overrides first
  if (manualWash) {
    if ((millis() / 5000) % 2 == 0) {
      digitalWrite(WASH_CW_PIN, HIGH);
      digitalWrite(WASH_CCW_PIN, LOW);
    } else {
      digitalWrite(WASH_CW_PIN, LOW);
      digitalWrite(WASH_CCW_PIN, HIGH);
    }
  } else if (manualSpin) {
    digitalWrite(SPIN_PIN, HIGH);
  } else if (manualFill) {
    digitalWrite(INLET_PIN, HIGH);
  } else if (manualDrain) {
    digitalWrite(DRAIN_PIN, HIGH);
  } else if (manualDryer) {
    digitalWrite(DRYER_PIN, HIGH);
  } else {
    allOff();
  }

  // Automatic cycle logic
  if (autoMode) {
    switch (washerState) {
      case W_FILLING:
        digitalWrite(INLET_PIN, HIGH);
        if (digitalRead(WATER_LEVEL_PIN) == HIGH) {
          washerState = W_WASHING;
          stateStartTime = millis();
          saveState();
        }
        break;

      case W_WASHING:
        if (((millis() - stateStartTime) / 5000) % 2 == 0) {
          digitalWrite(WASH_CW_PIN, HIGH);
          digitalWrite(WASH_CCW_PIN, LOW);
        } else {
          digitalWrite(WASH_CW_PIN, LOW);
          digitalWrite(WASH_CCW_PIN, HIGH);
        }
        if (millis() - stateStartTime > WASH_TIME) {
          washerState = W_DRAINING;
          stateStartTime = millis();
          saveState();
        }
        break;

      case W_DRAINING:
        digitalWrite(DRAIN_PIN, HIGH);
        if (digitalRead(WATER_LEVEL_PIN) == LOW) {
          washerState = W_SPINNING;
          stateStartTime = millis();
          saveState();
        }
        break;

      case W_SPINNING:
        digitalWrite(SPIN_PIN, HIGH);
        if (millis() - stateStartTime > SPIN_TIME) {
          washerState = W_DONE;
          autoMode = false;
          saveState();
        }
        break;

      default:
        break;
    }
  }
}

// Helper functions for human-readable states
const char* getWasherStateString() {
  switch (washerState) {
    case W_IDLE:     return "Idle";
    case W_FILLING:  return "Filling";
    case W_WASHING:  return "Washing";
    case W_DRAINING: return "Draining";
    case W_SPINNING: return "Spinning";
    case W_DONE:     return "Done";
    case W_ERROR:    return "Error";
    default:         return "Unknown";
  }
}

const char* getDryerStateString() {
  switch (dryerState) {
    case D_IDLE:    return "Idle";
    case D_RUNNING: return "Running";
    case D_DONE:    return "Done";
    case D_ERROR:   return "Error";
    default:        return "Unknown";
  }
}
