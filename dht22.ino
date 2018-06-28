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
#define DHTPIN 4     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors

DHT dht(DHTPIN, DHTTYPE);
bool readTemp = false;
int numberOfLoops = 10;
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
  tick.attach(1.0, checkTemp);
  
  pinMode(2, OUTPUT);
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
  
}
void checkTemp(){
  readTemp = true;
}
void loop() {
  delay(500);
  Serial.println("*** Looping ***");
  if(readTemp){
    readSensor();
    readTemp = false;
    digitalWrite(2, !digitalRead(2));
    // digitalWrite(led, !digitalRead(led));  
  }else{
    Serial.println("readTemp = false");
  }

  numberOfLoops--;
  if(numberOfLoops == 0){
    Serial.println("*** Goodnight ***");
    ESP.deepSleep(20e6);
  }
}
