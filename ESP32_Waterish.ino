#include <ETH.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <Thread.h>
#include <ThreadController.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <PubSubClientTools.h>
#include <FlowMeter.h>
#include <LiquidCrystal.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "settings.h"
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif
int menu = 0;
FlowMeter sensor[6] = FlowMeter();
int sensorstate[6];
int lastsensorstate[6];
int firstrun = 1;
Adafruit_MCP23017 mcp;
WiFiClient espwifi;
int value = 0;
const String s = "";
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
PubSubClient client("siwatsystem.com", 1883, espwifi);
PubSubClientTools mqtt(client);
boolean online = false;
boolean telemetryconnected = false;
void MeterCounter(int sensorid) {

}
void updateLCD(String maintext, int sensorvalue[6]) {
  if (enable_lcd) {
    switch (menu) {
      case '0' : //Status Home Screen
        lcd.clear();
        lcd.print(maintext);
        lcd.setCursor(0, 1);
        lcd.print(String(sensorvalue[0]) + " " + String(sensorvalue[1]) + " " + String(sensorvalue[2]) + " " + String(sensorvalue[3]) + " " + String(sensorvalue[4]) + " " + String(sensorvalue[5]));
        break;
      case '1' : //Admin Screen
        if (telemetryconnected)lcd.print("Telemetry: On");
        lcd.setCursor(0, 1);
        lcd.print("Node: " + nodename);
      case '2' :
        lcd.clear();
        lcd.print("SIWAT INC 2019 (C)");
        lcd.setCursor(0, 1);
        lcd.print("Waterish OS " + osversion);
    }
  }
}
void fakeInterupt(void *pvParameters) {
  //using fake interupt as I forgot to add hardware interupt to the pcb ToT
  while (1) {
    if (firstrun != 1)for (int counter = 0; counter <= 5; counter++)lastsensorstate[counter] = sensorstate[counter];
    for (int counter = 0; counter <= 5; counter++)sensorstate[counter] = mcp.digitalRead(counter);
    if (firstrun == 1)firstrun = 0;
    else {
      for (int counter = 0; counter <= 5; counter++) {
        if (lastsensorstate[counter] != sensorstate[counter]) {
          sensor[counter].count();
        }
      }
    }
  }
}
void sendstatemqtt(){
    for(int counter;counter<=6;counter++)mqtt.publish("/waterishos/node"+nodename+"/flowrate/"+counter,String(sensor[counter].getCurrentFlowrate()));
    for(int counter;counter<=6;counter++)mqtt.publish("/waterishos/node"+nodename+"/volume/"+counter,String(sensor[counter].getTotalVolume()));
}
void reportSensorState(void *pvParameters) {
  while (1) {
    delay(period);
    int flowrate[6];
    for (int counter = 0; counter <= 6; counter++)sensor[counter].tick(period);
    for (int counter = 0; counter <= 6; counter++)flowrate[counter] = sensor[counter].getCurrentFlowrate();
    if(online&&telemetryconnected)sendstatemqtt();
    updateLCD("Waterish Core", flowrate);
  }
}
void mqttResponder(void *pvParameters) {
  client.loop();
}
void setup() {
  Serial.begin(115200);
  pinMode(0, INPUT);
  if (enable_lcd)lcd.begin(16, 2);
  Serial.println("Booting");
  if (enable_lcd)lcd.print("Booting . . .");
  if (wifi) {
    WiFi.mode(WIFI_STA);
    delay(1000);
    if (enable_lcd)lcd.clear();
    if (enable_lcd)lcd.print("Connecting to WiFi . . .");
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Connection Failed!");
      if (enable_lcd)lcd.clear();
      if (enable_lcd)lcd.print("Connection Failed!");
      delay(5000);
      if (enable_lcd)lcd.clear();
      if (enable_lcd)lcd.print("Entering Offline Mode");
      delay(1000);
      online = false;
    } else
    {
      online = true;
    }
  } else {
    online = false;
  }
  if (wifi && telemetry && online) {
    if (client.connect("espwifi")) {
      Serial.println("connected");
      telemetryconnected = true;
    } else {
      Serial.println(s + "failed, rc=" + client.state());
      telemetryconnected = false;
    }
  } else {
    telemetryconnected = false;
  }
  Adafruit_MCP23017 mcp;
  mcp.begin();
  mcp.pinMode(0, INPUT);
  //Use Core 1 for Connection and General Process
  //Use Core 2 for Siwat INC(R) Fake Interupt
  xTaskCreatePinnedToCore(fakeInterupt, "fakeInterupt", 4096, NULL, 8, NULL, 0);
  xTaskCreatePinnedToCore(reportSensorState, "reportSensorState", 4096, NULL, 2, NULL, 1);
  if (telemetry && wifi)xTaskCreatePinnedToCore(mqttResponder, "mqttResponder", 4096, NULL, 2, NULL, 1);
}
void loop() {

}
