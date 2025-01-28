#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

// Konfigurasi WiFi
const char* ssid = "THINGSPEAK";
const char* password = "12345678";

// Konfigurasi ThingSpeak
const char* server = "api.thingspeak.com";
const unsigned long channelID = 2627849;
const char* writeAPIKey = "AEMRY2HNGAXF347G";

// Inisialisasi serial software
SoftwareSerial mySerial(5, 4); // RX, TX

// Inisialisasi WiFi dan ThingSpeak
WiFiClient client;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600); // 

  // Inisialisasi WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Kirim status ke Arduino Mega
  mySerial.println("WIFI_CONNECTED");

  // Inisialisasi ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  if (mySerial.available()) {
    String sensorData = mySerial.readStringUntil('\n');
    Serial.println("Received data: " + sensorData);

    // Parse data sensor
    int field1Index = sensorData.indexOf("field1=");
    int field2Index = sensorData.indexOf("&field2=");
    int field3Index = sensorData.indexOf("&field3=");
    int field4Index = sensorData.indexOf("&field4=");
    int field5Index = sensorData.indexOf("&field5=");  
    
    // Debug output untuk memeriksa posisi indeks
    Serial.println("field1Index: " + String(field1Index));
    Serial.println("field2Index: " + String(field2Index));
    Serial.println("field3Index: " + String(field3Index));
    Serial.println("field4Index: " + String(field4Index));
    Serial.println("field5Index: " + String(field5Index));

    if (field1Index != -1 && field2Index != -1 && field3Index != -1 && field4Index != -1 && field5Index != -1) {
      String suhu = sensorData.substring(field1Index + 7, field2Index);
      String kelembapan = sensorData.substring(field2Index + 8, field3Index);
      String kalibrasi = sensorData.substring(field3Index + 8, field4Index);
      String pressure = sensorData.substring(field4Index + 8, field5Index);
      String lux = sensorData.substring(field5Index + 8);  
      
      // Debug output untuk memeriksa nilai yang diparse
      Serial.println("Parsed suhu: " + suhu);
      Serial.println("Parsed kelembapan: " + kelembapan);
      Serial.println("Parsed kalibrasi: " + kalibrasi);
      Serial.println("Parsed pressure: " + pressure);
      Serial.println("Parsed lux: " + lux);

      // Konversi ke float dan kirim ke ThingSpeak
      ThingSpeak.setField(1, suhu.toFloat());
      ThingSpeak.setField(2, kelembapan.toFloat());
      ThingSpeak.setField(3, kalibrasi.toFloat());
      ThingSpeak.setField(4, pressure.toFloat());
      ThingSpeak.setField(5, lux.toFloat());  
      
      // Update ThingSpeak channel
      int httpCode = ThingSpeak.writeFields(channelID, writeAPIKey);
      if (httpCode == 200) {
        Serial.println("Data sent to ThingSpeak successfully");
      } else {
        Serial.println("Error sending data to ThingSpeak: " + String(httpCode));
      }
    } else {
      Serial.println("Error parsing sensor data");
    }
  }

  // Cek status koneksi WiFi
  if (WiFi.status() != WL_CONNECTED) {
    // Kirim status WiFi terputus ke Arduino Mega
    mySerial.println("WIFI_DISCONNECTED");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
    Serial.println("Reconnected to WiFi");
    mySerial.println("WIFI_CONNECTED");
  }

  delay(1000); 
}
