#ifndef CONSUMPTION_H
#define CONSUMPTION_H

#include <Arduino.h>

// Consumption tracking variables
extern float totalConsumption;        // Total consumption (L)
extern float dailyConsumption;        // Daily consumption (L)
extern float weeklyConsumption;       // Weekly consumption (L)
extern float monthlyConsumption;      // Monthly consumption (L)
extern float minuteConsumption;       // Current minute consumption (L)

// Time tracking
extern unsigned long lastMinuteReset;
extern unsigned long lastDayReset;
extern unsigned long lastWeekReset;
extern unsigned long lastMonthReset;

// Consumption limits
extern const float DAILY_LIMIT;
extern const float WEEKLY_LIMIT;
extern const float MONTHLY_LIMIT;
extern const float MINUTE_LIMIT;

// Alert flags
extern bool dailyLimitAlert;
extern bool weeklyLimitAlert;
extern bool monthlyLimitAlert;
extern bool minuteLimitAlert;

// Functions
void initConsumption();
void updateConsumption(float flowRate, unsigned long deltaTime);
void checkLimits();
void sendConsumptionData();
void resetTimePeriods();

#endif
