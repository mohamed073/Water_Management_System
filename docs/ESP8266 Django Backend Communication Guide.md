# ESP8266 Django Backend Communication Guide

This document outlines the HTTP API communication protocol between the ESP8266 device and Django backend server. The ESP8266 sends data via HTTP POST requests and receives commands in the HTTP response.



**Important Notes:**

- ESP8266 sends data every 5 seconds via HTTP POST
- All communication uses JSON format
- Content-Type: `application/json`
- HTTP timeout: 10 seconds
- Device ID: `water_system_001`

---

## Data Received from ESP8266 (POST Request)

The ESP8266 sends sensor data and system status to Django via HTTP POST requests.

### HTTP Request Structure

```http
Content-Type: application/json

{
  "device_id": "water_system_001",
  "timestamp": 123456789,
  "water_level": 75.5,
  "flow_rate_1": 12.3,
  "flow_rate_2": 8.7,
  "leak_detected": false,
  "consumption": {
    "total": 1500.25,
    "daily": 125.5,
    "weekly": 875.0,
    "monthly": 3500.75,
    "minute": 2.1
  },
  "alerts": {
    "daily_limit": false,
    "weekly_limit": false,
    "monthly_limit": false,
    "minute_limit": false,
    "leak_detected": false
  },
  "auto_mode": true,
  "data_age": 1500
}
```

### Data Fields Reference

| Field Name      | Data Type | Purpose                        | Arduino Source         | Hint                        |
| --------------- | --------- | ------------------------------ | ---------------------- | --------------------------- |
| `device_id`     | string    | Device identifier              | Fixed value            | Always "water_system_001"   |
| `timestamp`     | integer   | ESP8266 uptime in milliseconds | `millis()`             | System timestamp            |
| `water_level`   | float     | Tank water level               | `sendWaterLevelData()` | Percentage or actual level  |
| `flow_rate_1`   | float     | Main flow sensor reading       | `smoothedFlow1`        | L/min from main sensor      |
| `flow_rate_2`   | float     | Secondary flow sensor reading  | `smoothedFlow2`        | L/min from secondary sensor |
| `leak_detected` | boolean   | Leak detection status          | `checkForLeak()`       | true = leak detected        |

#### Consumption Object

| Field Name | Data Type | Purpose                     | Arduino Source        | Hint                  |
| ---------- | --------- | --------------------------- | --------------------- | --------------------- |
| `total`    | float     | Cumulative water usage      | `updateConsumption()` | Total liters consumed |
| `daily`    | float     | Daily consumption           | `updateConsumption()` | Resets daily          |
| `weekly`   | float     | Weekly consumption          | `updateConsumption()` | Resets weekly         |
| `monthly`  | float     | Monthly consumption         | `updateConsumption()` | Resets monthly        |
| `minute`   | float     | Per-minute consumption rate | `updateConsumption()` | Current flow rate     |

#### Alerts Object

| Field Name      | Data Type | Purpose                       | Arduino Source   | Hint                  |
| --------------- | --------- | ----------------------------- | ---------------- | --------------------- |
| `daily_limit`   | boolean   | Daily usage limit exceeded    | Alert system     | true = limit exceeded |
| `weekly_limit`  | boolean   | Weekly usage limit exceeded   | Alert system     | true = limit exceeded |
| `monthly_limit` | boolean   | Monthly usage limit exceeded  | Alert system     | true = limit exceeded |
| `minute_limit`  | boolean   | High flow rate detected       | Alert system     | true = possible leak  |
| `leak_detected` | boolean   | Duplicate of main leak status | `checkForLeak()` | Same as root level    |

#### System Status

| Field Name  | Data Type | Purpose                        | Arduino Source          | Hint                                   |
| ----------- | --------- | ------------------------------ | ----------------------- | -------------------------------------- |
| `auto_mode` | boolean   | Current operation mode         | `autoMode` variable     | true = automatic, false = manual       |
| `data_age`  | integer   | Time since last Arduino update | `millis() - lastUpdate` | Milliseconds, indicates data freshness |

---

## Commands Sent to ESP8266 (HTTP Response)

Django sends control commands back to the ESP8266 in the HTTP response to the POST request.

**NOTE** *have to send  "commands" key word before sending any data*

### HTTP Response Structure

```json
{
  "status": "success",
  "message": "Data received successfully",
  "commands": {
    "mode": "auto",
    "pump": "true",
    "valve1": "false",
    "valve2": "true",
    "valve3": "false"
  }
}
```

### Command Fields Reference

| Command  | Data Type | Purpose                 | Arduino Processing                              | Hint                              |
| -------- | --------- | ----------------------- | ----------------------------------------------- | --------------------------------- |
| `mode`   | string    | Switch operation mode   | `processReceivedData("mode:auto")`              | "auto" or "manual"                |
| `pump`   | string    | Control water pump      | `processReceivedData("motorTankroom:true")`     | "true" = run pump, "false" = stop |
| `valve1` | string    | Control main valve      | `processReceivedData("mainValveTankroom:true")` | "true" = open, "false" = close    |
| `valve2` | string    | Control tank valve      | `processReceivedData("tankValveTankroom:true")` | "true" = open, "false" = close    |
| `valve3` | string    | Control secondary valve | `processReceivedData("cadoValve:true")`         | "true" = open, "false" = close    |

### Command Processing Rules

1. **Mode Control**: Always processed regardless of current mode

   - `"auto"`: Enables automatic control, manual commands ignored
   - `"manual"`: Disables automatic control, all actuators reset to OFF

2. **Manual Commands**: Only processed when `auto_mode = false`

   - Commands are ignored in automatic mode
   - All commands use string values: `"true"` or `"false"`

   