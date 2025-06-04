#include "water_level.h"

// Define constants
const float TANK_HEIGHT = 9.0;       // cm tall tank
const float LOW_LEVEL = (3.0 / TANK_HEIGHT) * 100.0; // Fill when below 3cm
const float HIGH_LEVEL = (5.0 / TANK_HEIGHT) * 100.0; // Stop when above 5cm
const unsigned long VALVE_DELAY = 1000; // 1 sec between valve actions

// Define global variables
float waterLevel = 0;
bool pumpRunning = false;
byte fillStep = 0;
unsigned long lastActionTime = 0;

float getWaterLevel() {
  // Send ultrasonic pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Timeout set to ~26ms (~150cm max range)
  long duration = pulseIn(ECHO_PIN, HIGH, 26000); 
  
  float distance;
  if (duration == 0) {
    Serial.println("Error: Ultrasonic sensor timeout");
    return waterLevel; // Return previous valid value
  } else {
    distance = (duration * SOUND_SPEED) / 2.0;
  }
  
  waterLevel = max(0.0, TANK_HEIGHT - distance); // Ensure not negative
  
  // Calculate percentage
  float percentage = (waterLevel / TANK_HEIGHT) * 100.0;
  return constrain(percentage, 0.0, 100.0); // Ensure percentage is between 0 and 100
}

void fillTank() {
  
  unsigned long currentTime = millis();

  switch (fillStep) {
    case 0: // Waiting to fill
      if (waterLevel < LOW_LEVEL) {
        digitalWrite(VALVE_1, HIGH);
        Serial.println("Main valve opened");
        fillStep = 1;
        lastActionTime = currentTime;
      }
      break;
      
    case 1: // Open tank valve
      if (currentTime - lastActionTime >= VALVE_DELAY) {
        digitalWrite(VALVE_2, HIGH);
        Serial.println("Tank valve opened");
        fillStep = 2;
        lastActionTime = currentTime;
      }
      break;
      
    case 2: // Start pump
      if (currentTime - lastActionTime >= VALVE_DELAY) {
        digitalWrite(PUMP, HIGH);
        pumpRunning = true;
        Serial.println("Pump started");
        fillStep = 3;
      }
      break;
      
    case 3: // Monitoring fill level
      if (waterLevel > HIGH_LEVEL) {
        digitalWrite(PUMP, LOW);
        pumpRunning = false;
        Serial.println("Pump stopped");
        fillStep = 4;
        lastActionTime = currentTime;
      }
      break;
      
    case 4: // Close tank valve
      if (currentTime - lastActionTime >= VALVE_DELAY) {
        digitalWrite(VALVE_2, LOW);
        Serial.println("Tank valve closed");
        fillStep = 5;
        lastActionTime = currentTime;
      }
      break;
      
    case 5: // Close main valve
      if (currentTime - lastActionTime >= VALVE_DELAY) {
        digitalWrite(VALVE_1, LOW);
        Serial.println("Main valve closed");
        fillStep = 0;
        Serial.println("Filling complete");
      }
      break;
  }
}



void sendWaterLevelData() {
  waterLevel = getWaterLevel();
  Serial1.print("waterLevel:");  
  Serial1.println(waterLevel);

}
