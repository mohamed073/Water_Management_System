#ifndef WATER_LEVEL_H
#define WATER_LEVEL_H

#include <Arduino.h> 

#define ECHO_PIN 6
#define TRIG_PIN 7
#define SOUND_SPEED 0.034

#define PUMP 8
#define VALVE_1 9 // main valve
#define VALVE_2 10 // tank valve
#define VALVE_3 11

// Tank Settings
extern const float TANK_HEIGHT;       // cm tall tank
extern const float LOW_LEVEL;          // Fill when below 3cm
extern const float HIGH_LEVEL;         // Stop when above 5cm
extern const unsigned long VALVE_DELAY; // 1 sec between valve actions

extern float waterLevel; // Current water level in cm
extern bool pumpRunning; // Pump status
extern bool autoMode;    // Automatic mode status
extern byte fillStep;    // Filling step
extern unsigned long lastActionTime; // Last action time

void fillTank();
float getWaterLevel();
void sendWaterLevelData();

#endif // WATER_LEVEL_H
