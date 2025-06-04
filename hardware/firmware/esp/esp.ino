#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <SoftwareSerial.h>

// Communicate with arduino and esp
SoftwareSerial espSerial(D3, D2); // RX, TX

// WiFi & MQTT Configuration
const char* ssid = "B535_1569";         
const char* password = "tRnaLb3btgh"; 
const char* mqtt_server = "test.mosquitto.org"; // MQTT Broker

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

//  MQTT Topics
//esp subscribe on it. (Mobile send)
const char* motorTankroomTopic = "home/tankRoom/motor";
const char* tankValveTankroomTopic = "home/tankRoom/tankValve";
const char* mainValveTankroomTopic = "home/tankRoom/mainValve";
const char* cadoValveTopic = "home/tankRoom/cadoValve";
const char* automodeTopic = "home/tankRoom/automode"; // to decide manual or auto mode you work with 

 //esp publish on it.(Mobile receive)
 
const char* tankLevelTankroomTopic = "home/tankRoom/tankLevel";
const char* mainFlowrateTankroomTopic = "home/tankRoom/mainFlowrate";
const char* secondFlowrateTankroomTopic = "home/tankRoom/secondFlowrate";
const char* firstPIRSensorTopic = "home/lekageRoom/firstPIRSensor";
const char* secondPIRSensorTopic = "home/lekageRoom/secondPIRSensor";
const char* pressureSensorTopic = "home/pressureValue";
const char* leakTopic = "home/leakageRoom/leakAlarm";

/* Topics for voulume consumption*/
const char* totalConsumptionTopic = "home/waterConsumption/total";
const char* dailyConsumptionTopic = "home/waterConsumption/daily";
const char* weeklyConsumptionTopic = "home/waterConsumption/weekly";
const char* monthlyConsumptionTopic = "home/waterConsumption/monthly";
const char* minuteConsumptionTopic = "home/waterConsumption/minute";

/* topic for consumption alert*/
const char* dailyConsumptionAlertTopic = "home/waterConsumption/alert/daily";
const char* weeklyConsumptionAlertTopic = "home/waterConsumption/alert/weekly";
const char* monthlyConsumptionAlertTopic = "home/waterConsumption/alert/monthly";
const char* minuteConsumptionAlertTopic = "home/waterConsumption/alert/minute";

void setup_wifi() {
  delay(10);
  Serial.println("\nConnecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected, IP: " + WiFi.localIP().toString());
}

// Callback function: Prints received messages
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(" Received [");
  Serial.print(topic);
  Serial.print("]: ");

  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

    // Check topic type and print formatted message
   if (strcmp(topic, automodeTopic) == 0) {
    Serial.println("mode: " + message);
    espSerial.print("mode:");  
    espSerial.println(message);
  }
  else if (strcmp(topic, motorTankroomTopic) == 0) {
      Serial.println("Motor Tankroom Status: " + message);
      espSerial.print("motorTankroom:");  // Prefix message with topic
      espSerial.println(message);
  } 
  else if (strcmp(topic, tankValveTankroomTopic) == 0) {
      Serial.println("Tank Valve Tankroom: " + message);
      espSerial.print("tankValveTankroom:");  
      espSerial.println(message);
  } 
  else if (strcmp(topic, mainValveTankroomTopic) == 0) {
      Serial.println("Main Valve Tankroom: " + message);
      espSerial.print("mainValveTankroom:");  
      espSerial.println(message);
  } 
  else if (strcmp(topic, cadoValveTopic) == 0) {
      Serial.println("Cado Valve: " + message);
      espSerial.print("cadoValve:");  
      espSerial.println(message);
    }
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("Connected!");

      // 🔹 Subscribe to all control topics
      client.subscribe(motorTankroomTopic);
      client.subscribe(tankValveTankroomTopic);
      client.subscribe(mainValveTankroomTopic);
      client.subscribe(cadoValveTopic);
      client.subscribe(automodeTopic);
    } else {
      Serial.print(" Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 sec...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600); // Communication with Arduino buderate
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);  // set brocker & port number.
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
 
  client.loop();

  // data we will publish it after receiving it from arduino to all topics
  if (espSerial.available()) {
    String receivedData = espSerial.readStringUntil('\n');  // Example: "waterLevel:850.5"

    int colonIndex = receivedData.indexOf(':');
    if (colonIndex != -1) {  // Check if data is in correct format
      String dataType = receivedData.substring(0, colonIndex);
      String dataValue = receivedData.substring(colonIndex + 1);

      if (dataType == "waterLevel") {
            client.publish(tankLevelTankroomTopic, dataValue.c_str());
            Serial.print("water esp");
        } else if (dataType == "flowRate1") {
            client.publish(mainFlowrateTankroomTopic, dataValue.c_str());
        } else if (dataType == "flowRate2") {
            client.publish(secondFlowrateTankroomTopic, dataValue.c_str());
        } else if (dataType == "PIR1") {
            client.publish(firstPIRSensorTopic, dataValue.c_str());
        } else if (dataType == "PIR2") {
            client.publish(secondPIRSensorTopic, dataValue.c_str());
        } else if (dataType == "pressure") {
            client.publish(pressureSensorTopic, dataValue.c_str());
        } else if (dataType == "leakDetected") {
            client.publish(leakTopic, dataValue.c_str());
        } 
        else if (dataType == "totalConsumption") {
            client.publish(totalConsumptionTopic, dataValue.c_str());
        }  else if (dataType == "dailyConsumption") {
            client.publish(dailyConsumptionTopic, dataValue.c_str());
        }  else if (dataType == "weeklyConsumption") {
            client.publish(weeklyConsumptionTopic, dataValue.c_str());
        }  else if (dataType == "monthlyConsumption") {
            client.publish(monthlyConsumptionTopic, dataValue.c_str());
        }  else if (dataType == "minuteConsumption") {
            client.publish(minuteConsumptionTopic, dataValue.c_str());
        }
        else if (dataType == "dailyLimitAlert") {
            client.publish(dailyConsumptionAlertTopic, dataValue.c_str());
        }  else if (dataType == "weeklyLimitAlert") {
            client.publish(weeklyConsumptionAlertTopic, dataValue.c_str());
        }  else if (dataType == "monthlyLimitAlert") {
            client.publish(monthlyConsumptionAlertTopic, dataValue.c_str());
        }  else if (dataType == "minuteLimitAlert") {
            client.publish(minuteConsumptionAlertTopic, dataValue.c_str());
        }  
        else {
            Serial.println("Unknown data type received!");
        }
  }
}
}
