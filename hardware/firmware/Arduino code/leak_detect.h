#ifndef LEAK_DETECT_H
#define LEAK_DETECT_H

#include <Arduino.h>

extern bool isLeakDetected; // Tracks current leak status

#define FLOW_SENSOR_1 21
#define FLOW_SENSOR_2 20

#define PIR_SENSOR_1 24
#define PIR_SENSOR_2 25

#define ALARM 13

#define BASELINE_SAMPLES 30  // Number of samples to collect at startup


// Flow Sensor Variables
extern volatile int pulseCount1;
extern volatile int pulseCount2;
extern float flowRate1;
extern float flowRate2;

//used to statically(adaptively) control the water level
extern float diffBuffer[BASELINE_SAMPLES];
extern int bufferIndex;
extern bool baselineCollected;

extern float meanDiff;
extern float stdDevDiff;

// for the averaging filters 
extern float smoothedFlow1;
extern float smoothedFlow2;


void countPulses1();
void countPulses2();
float smooth(float newReading, float prevReading, float alpha = 0.2);
void checkForLeak(float rate1, float rate2);
void sendLeakData();

#endif // LEAK_DETECT_H
