#include <DHTesp.h>

#include <Adafruit_Sensor.h>

/**
 * Example for reading temperature and humidity
 * using the DHT22 and ESP8266
 *
 * Copyright (c) 2016 Losant IoT. All rights reserved.
 * https://www.losant.com
 * */
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#define SOILMOISTUREPIN 0     // Pin for soil moisture sensor
#define DHTPIN 4              // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22         // there are multiple kinds of DHT sensors
#define PUMP D3               // pin that controls the pump relay = red diode
#define SOILMOISTUREPOWER D4  // Power to the soil moisture sensor = blue diode
DHT dht(DHTPIN, DHTTYPE);
const char *ssId ="honey";
const char *password ="biffenbamse";

// const char *host = "www.90000.eu/gh/mcusd.php";
bool readTemp = false;
int numberOfLoopsBetweenWrites = 10;
int loopCounter = numberOfLoopsBetweenWrites;
float greenHouseTemp;
float greenHouseHumidity;
int soilMoisture;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while(!Serial) { }

  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  Serial.println("Running DHT!");
  Serial.println("-------------------------------------");
  delay(100);

  setupWifi();
    
  pinMode(2, OUTPUT);
  pinMode (D3, OUTPUT);
  pinMode (D4, OUTPUT);
}

void loop() {

  Serial.println("*** Looping ***");
  // readSensor();
  greenHouseHumidity = readGreenHouseHumidity();
  greenHouseTemp = readGreenHouseTemp();
  soilMoisture = readSoilMoisture();

  delay(10000);
  loopCounter--;
  if(loopCounter == 0){
    digitalWrite(D3, HIGH);
    
    Serial.println("Reading and sending data...");
    // Upload data
    postData(greenHouseTemp, greenHouseHumidity, soilMoisture);
    loopCounter  = numberOfLoopsBetweenWrites; 
    Serial.println("Reading and sending data done.");
    digitalWrite(D3, LOW);
  }
}

void setupWifi(){

  delay(1000);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssId, password);     //Connect to your WiFi router
  Serial.println("");
 
  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssId);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}

void goToSleep(){
    Serial.println("*** Goodnight ***");
    ESP.deepSleep(20e6);
}

void postData(float t, float ah, int sh){
  if((WiFi.status() != WL_CONNECTED)){
    setupWifi();
  }
  HTTPClient http;
  String d = "data={\"t\":" + String(t) + ",\"ah\":" + String(ah) +",\"sh\":" + String(sh) +"}";
  Serial.println("Data: " + d);
  http.begin("http://www.90000.eu/gh/mcusd.php");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int code = http.POST(d);
  String reply = http.getString();
  Serial.println("Reply code: " + String(code));
  Serial.println("Reply     : " + reply);
  http.end();
}
float readGreenHouseHumidity(){
  return dht.readHumidity();
}
float readGreenHouseTemp(){
  return dht.readTemperature();
}
int readSoilMoisture(){
  digitalWrite(D4, HIGH);
  delay(100);
  int m = analogRead(SOILMOISTUREPIN) + 5;
  digitalWrite(D4, LOW);
}

