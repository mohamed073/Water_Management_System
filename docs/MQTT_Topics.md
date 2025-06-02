#  MQTT  Topic  Structure

This document outlines the MQTT topics used in the system for communication between the ESP8266 device and the mobile application. The topics are categorized based on whether the ESP8266 subscribes to or publishes from them.

---

## Subscribed Topics (Mobile → ESP8266)

These topics are used by the mobile app to send commands to the ESP8266.

| Component       | Purpose                      | Topic Path                |
| --------------- | ---------------------------- | ------------------------- |
| Motor (Pump)    | Control water pump           | `home/tankRoom/motor`     |
| Tank Valve      | Control tank valve           | `home/tankRoom/tankValve` |
| Main Valve      | Control main water valve     | `home/tankRoom/mainValve` |
| Cado Valve      | Control secondary valve      | `home/tankRoom/cadoValve` |
| Automation Mode | Switch between manual & auto | `home/tankRoom/automode`  |

---

## Published Topics (ESP8266 → Mobile)

These topics are used by the ESP8266 to publish sensor data or system status updates to the mobile app.

| Sensor / Module       | Purpose                           | Topic Path                        |
| --------------------- | --------------------------------- | --------------------------------- |
| Tank Level Sensor     | Water level in the tank           | `home/tankRoom/tankLevel`         |
| Main Flow Rate Sensor | Flow rate of main water line      | `home/tankRoom/mainFlowrate`      |
| Secondary Flow Sensor | Flow rate of secondary line       | `home/tankRoom/secondFlowrate`    |
| PIR Sensor 1          | Motion detection - Zone 1         | `home/lekageRoom/firstPIRSensor`  |
| PIR Sensor 2          | Motion detection - Zone 2         | `home/lekageRoom/secondPIRSensor` |
| Pressure Sensor       | Water pressure reading            | `home/pressureValue`              |
| Leak Detection        | Alarm trigger if leakage detected | `home/leakageRoom/leakAlarm`      |



