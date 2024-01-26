#include <ArduinoJson.h>
#include <WiFi.h>
#include <AsyncMQTT_ESP32.h>
#include "secrets.h"

char payload[100];

TaskHandle_t sendSerialSignal;

// Assignment of pins for X1 connector
const int mainsFailurePin = 32;
const int loadOnInverterPin = 33;
const int stopOperationPin = 25;
const int loadOnMainsPin = 26;
const int generalAlarmPin = 27;
const int aucusticAlarmPin = 21;

AsyncMqttClient mqttClient;

const byte numChars = 46;
char receivedChars[numChars];

boolean newData = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Connecting to Wifi");
  WiFi.mode(WIFI_STA);  
  WiFi.begin(wifiSsid, wifiPassword);
  while(WiFi.status() != WL_CONNECTED){
      Serial.print(".");
      delay(100);
  }

  mqttClient.setCredentials(mqttUser, mqttPassword);
  mqttClient.setServer(MQTT_IP, MQTT_PORT);
  mqttClient.connect();

  pinMode(mainsFailurePin, INPUT);
  digitalWrite(mainsFailurePin, INPUT_PULLUP);
  pinMode(loadOnInverterPin, INPUT);
  digitalWrite(loadOnInverterPin, INPUT_PULLUP);
  pinMode(stopOperationPin, INPUT);
  digitalWrite(stopOperationPin, INPUT_PULLUP);
  pinMode(loadOnMainsPin, INPUT);
  digitalWrite(loadOnMainsPin, INPUT_PULLUP);
  pinMode(generalAlarmPin, INPUT);
  digitalWrite(generalAlarmPin, INPUT_PULLUP);
  pinMode(aucusticAlarmPin, INPUT);
  digitalWrite(aucusticAlarmPin, INPUT_PULLUP);

  xTaskCreatePinnedToCore(
      sendSerialSignalLoop, /* Function to implement the task */
      "sendSerialSignal", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &sendSerialSignal,  /* Task handle. */
      0); /* Core where the task should run */
}

void loop() {
  if(Serial.available()){
    recvWithStartEndMarkers();
    handleData();
  }
}

void sendSerialSignalLoop( void * parameter) {
  for(;;) {
    Serial.print("Q1<cr>");
    checkDryContacts();
    delay(4000);
  }
}

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '(';
  char endMarker = '<cr>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
          receivedChars[ndx] = rc;
          ndx++;
          if (ndx >= numChars) {
              ndx = numChars - 1;
          }
        }
        else {
          receivedChars[ndx] = '\0'; // terminate the string
          recvInProgress = false;
          ndx = 0;
          newData = true;
        }
      }

      else if (rc == startMarker) {
        recvInProgress = true;
      }
  }
}

void handleData() {
  if (newData) {
    String receivedCharsString = String(receivedChars);

    double inputVoltage = receivedCharsString.substring(0,5).toDouble();
    sprintf(payload, "%.2f", inputVoltage);
    mqttClient.publish("test/intputVoltage", 0, false, payload);

    double inputFaultVoltage = receivedCharsString.substring(6,11).toDouble();
    // sprintf(topic, "%s/inputFaultVoltage", deviceId);
    sprintf(payload, "%.2f", inputFaultVoltage);
    mqttClient.publish("test/inputFaultVoltage", 0, false, payload);

    double outputVoltage = receivedCharsString.substring(12,17).toDouble();
    // sprintf(topic, "%s/outputVoltage", deviceId);
    sprintf(payload, "%.2f", outputVoltage);
    mqttClient.publish("test/outputVoltage", 0, false, payload);

    int outputCurrent = receivedCharsString.substring(18, 21).toInt();
    // sprintf(topic, "%s/outputCurrent", deviceId);
    sprintf(payload, "%d", outputCurrent);
    mqttClient.publish("test/outputCurrent", 0, false, payload);

    double inputFreq = receivedCharsString.substring(22, 26).toDouble();
    // sprintf(topic, "%s/inputFreq", deviceId);
    sprintf(payload, "%.2f", inputFreq);
    mqttClient.publish("test/inputFrequency", 0, false, payload);

    double batteryVoltage = receivedCharsString.substring(27, 31).toDouble();
    // sprintf(topic, "%s/batteryVoltage", deviceId);
    sprintf(payload, "%.2f", batteryVoltage);
    mqttClient.publish("test/batteryVoltage", 0, false, payload);

    double upsTemp = receivedCharsString.substring(32, 36).toDouble();
    // sprintf(topic, "%s/upsTemp", deviceId);
    sprintf(payload, "%.2f", upsTemp);
    mqttClient.publish("test/upsTemp", 0, false, payload);

    bool beeperOn = receivedCharsString.substring(37, 38).toInt();
    // sprintf(topic, "%s/beeperOn", deviceId);
    sprintf(payload, "%d", beeperOn);
    mqttClient.publish("test/beeperOn", 0, false, payload);

    bool shutdownActive = receivedCharsString.substring(38, 39).toInt();
    // sprintf(topic, "%s/shutdownActive", deviceId);
    sprintf(payload, "%d", shutdownActive);
    mqttClient.publish("test/ShutdownActive", 0, false, payload);

    bool testInProgress = receivedCharsString.substring(39, 40).toInt();
    // sprintf(topic, "%s/testInProgress", deviceId);
    sprintf(payload, "%d", testInProgress);
    mqttClient.publish("test/testInProgress", 0, false, payload);

    bool upsStandby = receivedCharsString.substring(40, 41).toInt();
    // sprintf(topic, "%s/upsStandby", deviceId);
    sprintf(payload, "%d", upsStandby);
    mqttClient.publish("test/upsStandby", 0, false, payload);

    bool upsFailed = receivedCharsString.substring(41, 42).toInt();
    // sprintf(topic, "%s/upsFailed", deviceId);
    sprintf(payload, "%d", upsFailed);
    mqttClient.publish("test/upsFailed", 0, false, payload);

    bool buckActive = receivedCharsString.substring(42, 43).toInt();
    // sprintf(topic, "%s/buckActive", deviceId);
    sprintf(payload, "%d", buckActive);
    mqttClient.publish("test/buckActive", 0, false, payload);

    bool batteryLow = receivedCharsString.substring(43, 44).toInt();
    // sprintf(topic, "%s/batteryLow", deviceId);
    sprintf(payload, "%d", batteryLow);
    mqttClient.publish("test/batteryLow", 0, false, payload);

    bool utilityFail = receivedCharsString.substring(44).toInt();
    // sprintf(topic, "%s/utilityFail", deviceId);
    sprintf(payload, "%d", utilityFail);
    mqttClient.publish("test/utilityFail", 0, false, payload);


    // Serial.println(inputVoltage);
    // Serial.println(inputFaultVoltage);
    // Serial.println(outputVoltage);
    // Serial.println(ouputCurrent);
    // Serial.println(inputFreq);
    // Serial.println(batteryVoltage);
    // Serial.println(upsTemp);
    // Serial.println(beeperOn);
    // Serial.println(shutdownActive);
    // Serial.println(testInProgress);
    // Serial.println(upsStandby);
    // Serial.println(upsFailed);
    // Serial.println(buckActive);
    // Serial.println(batteryLow);
    // Serial.println(utilityFail);

    newData = false;
  }
}

void checkDryContacts(){

  int mainsFailure = digitalRead(mainsFailurePin);
  // sprintf(topic, "%s/mainsFailure", deviceId);
  sprintf(payload, "%d", mainsFailure);
  mqttClient.publish("test/mainsFailure", 0, false, payload);

  int loadOnInverter = digitalRead(loadOnInverterPin);
  // sprintf(topic, "%s/loadOnInverter", deviceId);
  sprintf(payload, "%d", loadOnInverter);
  mqttClient.publish("test/loadOnInverter", 0, false, payload);

  int stopOperation = digitalRead(stopOperationPin);
  // sprintf(topic, "%s/stopOperation", deviceId);
  sprintf(payload, "%d", stopOperation);
  mqttClient.publish("test/stopOperation", 0, false, payload);

  int loadOnMains = digitalRead(loadOnMainsPin);
  // sprintf(topic, "%s/loadOnMains", deviceId);
  sprintf(payload, "%d", loadOnMains);
  mqttClient.publish("test/loadOnMains", 0, false, payload);

  int generalAlarm = digitalRead(generalAlarmPin);
  // sprintf(topic, "%s/generalAlarm", deviceId);
  sprintf(payload, "%d", generalAlarm);
  mqttClient.publish("test/generalAlarm", 0, false, payload);

  int aucusticAlarm = digitalRead(aucusticAlarmPin);
  // sprintf(topic, "%s/aucusticAlarm", deviceId);
  sprintf(payload, "%d", aucusticAlarm);
  mqttClient.publish("test/acusticAlarm", 0, false, payload);
}
