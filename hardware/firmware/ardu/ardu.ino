#include<avr/wdt.h>

#include "water_level.h"
#include "leak_detect.h"
#include  "volume_consumption.h"

unsigned long lastTime = 0;
bool autoMode = true; // Start in automatic mode

void setup() {
  wdt_enable(WDTO_8S); // enable watch dog timer with 8 seconds
  
  Serial.begin(9600);  // Use the default Serial for communication with ESP8266

  pinMode(FLOW_SENSOR_1, INPUT);
  pinMode(FLOW_SENSOR_2, INPUT);
  pinMode(PIR_SENSOR_1, INPUT);
  pinMode(PIR_SENSOR_2, INPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(PUMP, OUTPUT);
  pinMode(VALVE_1, OUTPUT);
  pinMode(VALVE_2, OUTPUT);
  pinMode(VALVE_3, OUTPUT);
  
  pinMode(ALARM, OUTPUT);

  digitalWrite(PUMP, LOW);
  digitalWrite(VALVE_1, LOW);
  digitalWrite(VALVE_2, LOW);
  digitalWrite(VALVE_3, LOW);
  
  digitalWrite(ALARM, LOW);

  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_1), countPulses1, RISING);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_2), countPulses2, RISING);
  
  initConsumption();//Initalize conusmption Tracking
}

void processReceivedData(String receivedData) {
  int colonIndex = receivedData.indexOf(':');
  if (colonIndex != -1) {
    String dataType = receivedData.substring(0, colonIndex);
    String dataValue = receivedData.substring(colonIndex + 1);
    
  if (dataType == "mode") {
    dataValue.trim();
    if (dataValue.equalsIgnoreCase("true")) { 
      autoMode = true;
      Serial.println("Mode set to AUTO");
    } else if (dataValue.equalsIgnoreCase("false")) { //reset actuators before switching to manual mode
      digitalWrite(PUMP, LOW);
      digitalWrite(VALVE_1, LOW);
      digitalWrite(VALVE_2, LOW);
      digitalWrite(VALVE_3, LOW);
      autoMode = false;
      Serial.println("Mode set to MANUAL");
    }
  } 

    // If in AUTO mode, ignore manual commands
  if (!autoMode) {
    if (dataType == "motorTankroom") {
      //dataValue.trim();  
      int motorStatus = (dataValue.equalsIgnoreCase("true")) ? HIGH : LOW;
  
      Serial.print("Motor Tankroom Status: ");
      Serial.println(motorStatus == HIGH ? "ON" : "OFF");
  
      digitalWrite(PUMP, motorStatus);
      } 
      else if (dataType == "mainValveTankroom") {
        dataValue.trim();
        int mainValveStatus = (dataValue.equalsIgnoreCase("true")) ? HIGH : LOW;
  
        Serial.print("Main Valve Tankroom: ");
        Serial.println(mainValveStatus == HIGH ? "ON" : "OFF");
  
        digitalWrite(VALVE_1, mainValveStatus);
      }
      else if (dataType == "tankValveTankroom") {
        dataValue.trim();
        int tankValveStatus = (dataValue.equalsIgnoreCase("true")) ? HIGH : LOW;
  
        Serial.print("Tank Valve Tankroom: ");
        Serial.println(tankValveStatus == HIGH ? "ON" : "OFF");
  
        digitalWrite(VALVE_2, tankValveStatus);
      }
      else if (dataType == "cadoValve") {
        dataValue.trim();
        int cadoValveStatus = (dataValue.equalsIgnoreCase("true")) ? HIGH : LOW;
  
        Serial.print("CadoValve: ");
        Serial.println(cadoValveStatus == HIGH ? "ON" : "OFF");
  
        digitalWrite(VALVE_3, cadoValveStatus);
      }
      else {
        Serial.println("Unknown DataType!");
      }
    }
  }
}

void loop() {
  wdt_reset(); // Reset watchdog timer

  unsigned long currentMillis = millis();
  
  if (currentMillis - lastTime >= 1000) {
    
    // calculate time diffrence for consumption.
    unsigned long deltaTime = currentMillis - lastTime;
    
    flowRate1 = (pulseCount1 / 7.5);  // Convert pulses to L/min
    flowRate2 = (pulseCount2 / 7.5);
    smoothedFlow1 = smooth(flowRate1, smoothedFlow1);// filter the flow sensors readings
    smoothedFlow2 = smooth(flowRate2, smoothedFlow2);

    pulseCount1 = 0;
    pulseCount2 = 0;

    //update consumption tracking using main flow sensor (sensor 1)
    updateConsumption(smoothedFlow1, deltaTime);
    
   

    if (autoMode){ //Automatic tank control and Check for leaks 
      fillTank(); 
      checkForLeak(smoothedFlow1, smoothedFlow2); 
    }
    
    // Send structured data to ESP8266
     
    //int pir1 = digitalRead(PIR_SENSOR_1);
    //int pir2 = digitalRead(PIR_SENSOR_2);
    
//    Serial.print("PIR1:");
//    Serial.println(pir1 == HIGH ? "true" : "false");
//    Serial.print("PIR2:");
//    Serial.println(pir2 == HIGH ? "true" : "false");

    //water level Data
    sendWaterLevelData();
    //Send Leakge Data
    sendLeakData();
    //Send consumption Data
    sendConsumptionData();

    lastTime = currentMillis;
  }

  // Receiving data from ESP8266
  if (Serial1.available()) {
    String receivedData = Serial1.readStringUntil('\n');
    Serial.println("Received: " + receivedData);
    processReceivedData(receivedData);
  }
}
