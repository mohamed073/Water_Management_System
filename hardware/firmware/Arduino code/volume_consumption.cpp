#include "volume_consumption.h"

// Consumption tracking variables
float totalConsumption = 0.0;
float dailyConsumption = 0.0;
float weeklyConsumption = 0.0;
float monthlyConsumption = 0.0;
float minuteConsumption = 0.0;

// Time tracking (in milliseconds)
unsigned long lastMinuteReset = 0;
unsigned long lastDayReset = 0;
unsigned long lastWeekReset = 0;
unsigned long lastMonthReset = 0;

// Consumption limits (adjust these values as needed)
const float DAILY_LIMIT = 500.0;     // 500L per day
const float WEEKLY_LIMIT = 3000.0;   // 3000L per week
const float MONTHLY_LIMIT = 12000.0; // 12000L per month
const float MINUTE_LIMIT = 0.75;     // 10L per minute

// Alert flags
bool dailyLimitAlert = false;
bool weeklyLimitAlert = false;
bool monthlyLimitAlert = false;
bool minuteLimitAlert = false;

void initConsumption() {
  unsigned long currentTime = millis();
  lastMinuteReset = currentTime;
  lastDayReset = currentTime;
  lastWeekReset = currentTime;
  lastMonthReset = currentTime;
  
  Serial.println("Consumption tracking initialized");
}

void updateConsumption(float flowRate, unsigned long deltaTime) {
  // Convert deltaTime from milliseconds to minutes
  float deltaMinutes = deltaTime / 60000.0;
  
  // Calculate volume consumed (L)
  float volumeConsumed = flowRate * deltaMinutes;
  
  // Update all consumption counters
  totalConsumption += volumeConsumed;
  dailyConsumption += volumeConsumed;
  weeklyConsumption += volumeConsumed;
  monthlyConsumption += volumeConsumed;
  minuteConsumption += volumeConsumed;
  
  // Check and reset time periods
  resetTimePeriods();
  
  // Check consumption limits
  checkLimits();
}

void resetTimePeriods() {
  unsigned long currentTime = millis();
  
  // Reset minute consumption (every 60 seconds)
  if (currentTime - lastMinuteReset >= 60000) {
    minuteConsumption = 0.0;
    lastMinuteReset = currentTime;
    minuteLimitAlert = false;
  }
  
  // Reset daily consumption (every 24 hours)
  if (currentTime - lastDayReset >= 86400000) {
    dailyConsumption = 0.0;
    lastDayReset = currentTime;
    dailyLimitAlert = false;
  }
  
  // Reset weekly consumption (every 7 days)
  if (currentTime - lastWeekReset >= 604800000) {
    weeklyConsumption = 0.0;
    lastWeekReset = currentTime;
    weeklyLimitAlert = false;
  }
  
  // Reset monthly consumption (every 30 days)
  if (currentTime - lastMonthReset >= 2592000000UL) {
    monthlyConsumption = 0.0;
    lastMonthReset = currentTime;
    monthlyLimitAlert = false;
  }
}

void checkLimits() {
  // Check minute limit
  if (minuteConsumption > MINUTE_LIMIT && !minuteLimitAlert) {
    minuteLimitAlert = true;
    Serial.println("ALERT: Minute consumption limit exceeded!");
  }
  
  // Check daily limit
  if (dailyConsumption > DAILY_LIMIT && !dailyLimitAlert) {
    dailyLimitAlert = true;
    Serial.println("ALERT: Daily consumption limit exceeded!");
  }
  
  // Check weekly limit
  if (weeklyConsumption > WEEKLY_LIMIT && !weeklyLimitAlert) {
    weeklyLimitAlert = true;
    Serial.println("ALERT: Weekly consumption limit exceeded!");
  }
  
  // Check monthly limit
  if (monthlyConsumption > MONTHLY_LIMIT && !monthlyLimitAlert) {
    monthlyLimitAlert = true;
    Serial.println("ALERT: Monthly consumption limit exceeded!");
  }
}

void sendConsumptionData() {
  // Send consumption data
  Serial.print("totalConsumption:");
  Serial.println(totalConsumption, 2);
  
  Serial.print("dailyConsumption:");
  Serial.println(dailyConsumption, 2);
  
  Serial.print("weeklyConsumption:");
  Serial.println(weeklyConsumption, 2);
  
  Serial.print("monthlyConsumption:");
  Serial.println(monthlyConsumption, 2);
  
  Serial.print("minuteConsumption:");
  Serial.println(minuteConsumption, 2);
  
  // Send alert flags
  if (dailyLimitAlert == "true") {
    Serial.print("dailyLimitAlert:");
    Serial.println("true");
  }
  if (weeklyLimitAlert == "true") {
    Serial.print("weeklyLimitAlert:");
    Serial.println("true");
  } 
  if (monthlyLimitAlert == "true") {
    Serial.print("monthlyLimitAlert:");
    Serial.println("true");
  }
  
  if (minuteLimitAlert == "true") {
    Serial.print("minuteLimitAlert:");
    Serial.println("true");
  }
    
}
  
