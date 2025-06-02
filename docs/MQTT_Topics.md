#  MQTT  Topic  Structure

This document outlines the MQTT topics used in the system for communication between the ESP8266 device and the mobile application. The topics are categorized based on whether the ESP8266 subscribes to or publishes from them.

---

## Subscribed Topics (Mobile → ESP8266)

These topics are used by the mobile app to send commands to the ESP8266 (MicroController).

| Component       | Purpose                      | Data Type        | Topic Path                | Hint                                                         |
| :-------------- | :--------------------------- | :--------------- | :------------------------ | :----------------------------------------------------------- |
| Motor (Pump)    | Control water pump           | "true" / "false" | `home/tankRoom/motor`     | "**true**": means run the motor                              |
| Tank Valve      | Control tank valve           | "true" / "false" | `home/tankRoom/tankValve` | "**true**": means open the valve                             |
| Main Valve      | Control main water valve     | "true" / "false" | `home/tankRoom/mainValve` | "**true**": means open the valve                             |
| Cado Valve      | Control secondary valve      | "true" / "false" | `home/tankRoom/cadoValve` | "**true**": means open the valve                             |
| Automation Mode | Switch between manual & auto | "true" / "false" | `home/tankRoom/automode`  | "**true**": means make the system on Automatic Mode, User cannot send commands/ Control the system except switching to Manual Mode (**false**) |

---

## Published Topics (ESP8266 → Mobile)

These topics are used by the ESP8266 to publish sensor data or system status updates to the mobile app.

1. | Sensor / Module       | Purpose                           | Data Type        | Topic Path                        | Hint                                                         |
   | --------------------- | --------------------------------- | ---------------- | --------------------------------- | ------------------------------------------------------------ |
   | Tank Level Sensor     | Water level in the tank           | float number     | `home/tankRoom/tankLevel`         | the float number will be send as a *string* you will have to **parse it** first |
   | Main Flow Rate Sensor | Flow rate of main water line      | float number     | `home/tankRoom/mainFlowrate`      | the float number will be send as a *string* you will have to **parse it** first |
   | Secondary Flow Sensor | Flow rate of secondary line       | float number     | `home/tankRoom/secondFlowrate`    | the float number will be send as a *string* you will have to **parse it** first |
   | PIR Sensor 1          | Motion detection - Zone 1         | "true" / "false" | `home/lekageRoom/firstPIRSensor`  | **true**: means there is a person                            |
   | PIR Sensor 2          | Motion detection - Zone 2         | "true" / "false" | `home/lekageRoom/secondPIRSensor` | **true**: means there is a person                            |
   | Leak Detection        | Alarm trigger if leakage detected | "true" / "false" | `home/leakageRoom/leakAlarm`      | **true**: means there is a leakage                           |



