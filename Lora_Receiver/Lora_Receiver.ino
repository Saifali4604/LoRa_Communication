// Reciver
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows

const int csPin = 4;     
const int resetPin = 5;    
const int DI0 = 2;   

int localAddress = 66;  
int destination = 55;

StaticJsonDocument<256> doc;
String recv_jsondata; 
String send_jsondata; 

void setup() {
  Serial.begin(9600);
  lcd.init(); // initialize the lcd
  lcd.backlight();
  lcd.clear();
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
  onReceive();
}

void onReceive() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.println("Message for me! Processing...");
    recv_jsondata = "";  // Reset buffer before reading
    while (LoRa.available()) {
      //Serial.print((char)LoRa.read());
      recv_jsondata += (char)LoRa.read();
    }

    DeserializationError error = deserializeJson(doc, recv_jsondata);
    if (!error) {
      int Addr_From = doc["Addr_From"];
      int Addr_To = doc["Addr_To"];
      int Temperature = doc["T"];
      int Humidity = doc["H"];

      if (Addr_To == localAddress) { 
        lcd.setCursor(0, 0);
        lcd.print("Temp:");
        lcd.print(Temperature);
        lcd.print(" C");
        lcd.setCursor(0, 1);
        lcd.print("Hum:");
        lcd.print(Humidity);
        lcd.print(" %"); 
        Serial.println("Message for me! Processing...");
        Serial.print("From Address: "); Serial.println(Addr_From);
        Serial.print("Temperature: "); Serial.print(Temperature); Serial.println(" °C");
        Serial.print("Humidity: "); Serial.print(Humidity); Serial.println(" %");
      } else {
        Serial.println("Message not for me. Ignoring...");
      }
    } else {
      Serial.println("JSON Parsing Failed");
    }
    delay(1000);
  }
}
