#include "DHT.h"

int dhtPin = 3;
#define DHTTYPE DHT22

DHT dht(dhtPin, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  float suhu = dht.readTemperature();
  float kelembapan = dht.readHumidity();

  if (isnan(suhu) || isnan(kelembapan)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.println("Suhu: " + String(suhu) + " *C");
    Serial.println("Kelembapan: " + String(kelembapan) + " %");
    Serial.println();
  }
  delay(1000);
}
