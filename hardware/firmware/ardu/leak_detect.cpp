#include "leak_detect.h"

unsigned long baselineStartTime = 0;
const unsigned long BASELINE_TIMEOUT = 60000; // 60 seconds timeout

bool isLeakDetected = false; // Tracks current leak status

float smoothedFlow1 = 0.0;
float smoothedFlow2 = 0.0;

// Flow Sensor Variables
volatile int pulseCount1 = 0;
volatile int pulseCount2 = 0;
float flowRate1 = 0.0;
float flowRate2 = 0.0;

#define BASELINE_SAMPLES 30  // Number of samples to collect at startup
float diffBuffer[BASELINE_SAMPLES];
int bufferIndex = 0;
bool baselineCollected = false;

float meanDiff = 0.0;
float stdDevDiff = 0.0;



// Interrupt Functions for Flow Sensors
void countPulses1() { pulseCount1++; }
void countPulses2() { pulseCount2++; }

//This is a signal filtering technique used to reduce short-term fluctuations (noise) in sensor readings
float smooth(float newReading, float prevReading, float alpha = 0.2) {
  return alpha * newReading + (1 - alpha) * prevReading;
}

void checkForLeak(float rate1, float rate2) {
  
  float difference = fabs(rate1 - rate2);

  if (!baselineCollected) {
    
    // Collect baseline samples
    if (bufferIndex < BASELINE_SAMPLES) {
      diffBuffer[bufferIndex++] = difference;
      return;  // Skip leak check until baseline is complete
    } else {
      // Calculate mean
      float sum = 0;
      for (int i = 0; i < BASELINE_SAMPLES; i++) {
        sum += diffBuffer[i];
      }
      meanDiff = sum / BASELINE_SAMPLES;

      // Calculate standard deviation
      float sqSum = 0;
      for (int i = 0; i < BASELINE_SAMPLES; i++) {
        float diff = diffBuffer[i] - meanDiff;
        sqSum += diff * diff;
      }
      stdDevDiff = sqrt(sqSum / BASELINE_SAMPLES);
      
      
      if (stdDevDiff < 0.01) {// Ensure minimum standard deviation to prevent division by zero
        stdDevDiff = 0.01;
      }

      baselineCollected = true;
      Serial.print("Baseline complete: Mean=");
      Serial.print(meanDiff);
      Serial.print(", StdDev=");
      Serial.println(stdDevDiff);
    }
  }

  if (baselineCollected) {
    // Adaptive threshold using z-score
    const float Z_THRESHOLD = 2.5;  // Adjust based on sensitivity
    float zScore = fabs(difference - meanDiff) / stdDevDiff;
  
    if (zScore > Z_THRESHOLD) {
      Serial.println("Leak detected!");
      digitalWrite(ALARM, HIGH); // Activate alarm
      isLeakDetected = true;
    } else {
      Serial.println("No leak detected.");
      digitalWrite(ALARM, LOW); // Deactivate alarm
      isLeakDetected = false;
     }
  }
}

void sendLeakData() {
  
  Serial1.print("flowRate1:");
  Serial1.println(smoothedFlow1);
  Serial1.print("flowRate2:");
  Serial1.println(smoothedFlow2);
  Serial1.print("leakDetected:");
  Serial1.println(isLeakDetected ? "true" : "false");
  
}
