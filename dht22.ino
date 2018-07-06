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

#define SOILMOISTUREPIN A0     // Pin for soil moisture sensor
#define DHTPIN 4              // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22         // there are multiple kinds of DHT sensors
#define PUMP D3               // pin that controls the pump relay = red diode
#define SOILMOISTUREPOWER D4  // Power to the soil moisture sensor = blue diode
DHT dht(DHTPIN, DHTTYPE);
const char *ssId ="honey";
const char *password ="biffenbamse";

// const char *host = "www.90000.eu/gh/mcusd.php";
bool readTemp = false;
int numberOfLoopsBetweenWrites = 100;
int loopCounter = numberOfLoopsBetweenWrites;
float greenHouseTemp;
float greenHouseHumidity;
int soilMoisture;
int soilMoisturePin = A0;

bool pumpIsOn = false;
const unsigned long timeBetweenPosts_ms = 1 * 60 * 60 * 1000;  // Once every hour
const unsigned long timeBetweenSensorReads_ms = 0.5 * 60 * 60 * 1000; // Once every half hour
const unsigned long timeBetweenChecks_ms = 1 * 60 * 60 * 1000; // Once every hour
const unsigned long timeToRunPump_ms = 30 * 1000; // 30s



unsigned long startPost_ms;
unsigned long startRead_ms;
unsigned long startCheck_ms;
unsigned long turnPumpOff_ms;

unsigned long current_ms;

ESP8266WebServer server(8081);

void setup() {
  pinMode(2, OUTPUT); 
  pinMode (D3, OUTPUT);
  pinMode (D4, OUTPUT);

  digitalWrite(D4, LOW);
  digitalWrite(D3, LOW);

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
    
  server.on("/", [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/html", "");
  });
  server.on("/status",[](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/html", pumpIsOn?"1":"0");
  });
  server.on("/flip",[](){
    setPump(!pumpIsOn);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/html", pumpIsOn?"1":"0"  );
  });
  greenHouseHumidity = readGreenHouseHumidity();
  greenHouseTemp = readGreenHouseTemp();
  soilMoisture = readSoilMoisture();

  startRead_ms = millis();
  startPost_ms = millis();
  startCheck_ms = millis();
  
  server.begin();
}
void loop() {

  server.handleClient();
  current_ms = millis();

  if(pumpIsOn){
    if(current_ms - turnPumpOff_ms >= timeToRunPump_ms){
      setPump(false);
    }
  }

  if(current_ms - startRead_ms >= timeBetweenSensorReads_ms){
    startRead_ms = current_ms;
    
    Serial.println("Reading sensors");
    greenHouseHumidity = readGreenHouseHumidity();
    greenHouseTemp = readGreenHouseTemp();
    soilMoisture = readSoilMoisture();
  }
  if(current_ms - startPost_ms >= timeBetweenPosts_ms){
    startPost_ms = current_ms;
    Serial.println("Posting data");
    postData(greenHouseTemp, greenHouseHumidity, soilMoisture);
  }
  if(current_ms - startCheck_ms >= timeBetweenChecks_ms){
    startCheck_ms = current_ms;
    Serial.println("Checking if I should turn the pump on");
    if(checkLogic()){
      setPump(true);
    }
  }
  delay(100);
}
bool checkLogic(){
  if((WiFi.status() != WL_CONNECTED)){
    setupWifi();
  }
  HTTPClient http;
  http.begin("http://www.90000.eu/gh/q.php");
  int code = http.GET();
  String reply = http.getString();
  Serial.println("Reply code: " + String(code));
  Serial.println("Reply     : " + reply);
  http.end();
  return reply == "1" && code == 200;;  
}

bool setPump(bool onOrOff){
  if(onOrOff){
    pumpIsOn = true;
    turnPumpOff_ms = millis();
    digitalWrite(D3, HIGH);
  }else{
    pumpIsOn = false;
    digitalWrite(D3, LOW);
  }
  Serial.println("Pump is on: " + String(pumpIsOn));
  return pumpIsOn;
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
  Serial.println("Starting web server");
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
  float numberOfLoops = 5;
  float sum = 0;
  for(int i = 0; i< numberOfLoops; i++){
    sum+=dht.readHumidity();
    delay(2000);
  }
  sum = sum/numberOfLoops;
  Serial.println("Humidity: " + String(sum));
  return sum;
}
float readGreenHouseTemp(){
  float sum = 0;
  float numberOfLoops = 5;
  for(int i=0; i< numberOfLoops; i++){
    sum += dht.readTemperature();
    delay(2000);
  }
  sum = sum/numberOfLoops;
  Serial.println("Temp: " + String(sum));
  return sum;
}
int readSoilMoisture(){
  digitalWrite(D4, HIGH);
  delay(10);
  int numberOfLoops = 5;
  int m=0;
  for(int i=0; i<numberOfLoops;i++){
    m += analogRead(soilMoisturePin);
    delay(100);
  }
  m = m/numberOfLoops;
  digitalWrite(D4, LOW);
  Serial.println("Soil moisture: " + String(m));
  return m;
}

