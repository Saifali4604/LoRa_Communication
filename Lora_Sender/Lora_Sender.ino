// Sender
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include "DHT.h"

const int csPin = 22;     
const int resetPin = 5;    
const int DI0 = 2;    

int localAddress = 55;  
int destination = 66;  
long lastSendTime = 0;
int interval = 2000;

#define DHTPIN  4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

StaticJsonDocument<256> doc;
String send_jsondata;
String recv_jsondata; 

void setup() {
  Serial.begin(9600);
  dht.begin();
  while (!Serial);

  Serial.println("LoRa Duplex");
  LoRa.setPins(csPin, resetPin, DI0);

  if (!LoRa.begin(433E6)) {  
    Serial.println("LoRa init failed. Check connections.");
    while (true);
  }

  Serial.println("LoRa init succeeded.");
}

void loop() {
  if (millis() - lastSendTime > interval) {
    Send_data();
    lastSendTime = millis();
  }
}

void Send_data() {
  int temp = dht.readTemperature();
  int hum = dht.readHumidity();
  
  Serial.print("Temperature: "); Serial.print(temp);
  Serial.print(" | Humidity: "); Serial.println(hum);

  doc["Addr_From"] = localAddress; 
  doc["Addr_To"] = destination;    
  doc["T"] = temp; 
  doc["H"] = hum;     

  send_jsondata = "";  // Reset JSON string
  serializeJson(doc, send_jsondata);

  LoRa.beginPacket();    
  LoRa.print(send_jsondata);  
  LoRa.endPacket();        
  
  Serial.println("Data sent!");
}


