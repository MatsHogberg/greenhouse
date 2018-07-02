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
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#define SOILMOISTUREPIN 0 // Pin for soil moisture sensor
#define DHTPIN 4     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors

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
Ticker tick;
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
}

void loop() {

  Serial.println("*** Looping ***");
  // readSensor();
  greenHouseHumidity = readGreenHouseHumidity();
  greenHouseTemp = readGreenHouseTemp();
  soilMoisture = readSoilMoisture();
  /*
  if(readTemp){
    readSensor();
    readTemp = false;
    digitalWrite(2, !digitalRead(2));
    // digitalWrite(led, !digitalRead(led));  
  }else{
    Serial.println("readTemp = false");
  }

  numberOfLoopsBetweenWrites--;
  if(numberOfLoopsBetweenWrites == 0){
    // goToSleep();
  }
  */
  delay(10000);
  loopCounter--;
  if(loopCounter == 0){
    Serial.println("Reading and sending data...");
    // Upload data
    postData(greenHouseTemp, greenHouseHumidity, soilMoisture);
    loopCounter  = numberOfLoopsBetweenWrites; 
    Serial.println("Reading and sending data done.");
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
    return analogRead(SOILMOISTUREPIN) + 5;
}

void readSensor(){
   float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.print(hif);
    Serial.println(" *F");

    postData(t, h, h);
}

