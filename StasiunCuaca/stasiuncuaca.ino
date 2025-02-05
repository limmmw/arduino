#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_TSL2561_U.h>

// Inisialisasi LCD dengan alamat I2C 0x27, lebar 16 karakter, tinggi 2 baris
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Inisialisasi sensor DHT22
int dhtPin = 3;
#define DHTTYPE DHT22
DHT dht(dhtPin, DHTTYPE);

// Inisialisasi RTC DS3231
RTC_DS3231 rtc;

// Inisialisasi anemometer
#define windPin 2
const float pi = 3.14159265;
int period = 10000; // Measurement period (miliseconds)
int radio = 90; // Distance from center windmill to outer cup (mm)
int jml_celah = 18; // jumlah celah sensor
volatile unsigned int counter = 0;
unsigned int RPM = 0;
float kalibrasi = 0;

// Inisialisasi buzzer
const int buzzerPin = 4;

// Inisialisasi BMP280
Adafruit_BMP280 bmp;

// Inisialisasi TSL2561
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

// Inisialisasi LED
const int ledPin = 5;

// Variabel untuk timing
unsigned long previousMillis = 0;
const long interval = 1000; 

// Variabel sensor
float suhu;
float kelembapan;
uint16_t lux; 

// Variabel untuk status WiFi
bool wifiConnected = false;

// Variabel untuk kondisi cuaca sebelumnya
String previousKondisiCuaca = "";
String previousWaktuStr = ""; 

// Daftar hari
const char* daysOfTheWeek[] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};

// Fungsi interrupt untuk anemometer
void addcount() {
  counter++;
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600); // Serial1 untuk komunikasi dengan ESP8266

  // Inisialisasi RTC
  if (!rtc.begin()) {
    while (1) delay(10);
  }
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // Inisialisasi sensor DHT
  dht.begin();
  
  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  
  // Inisialisasi anemometer
  pinMode(windPin, INPUT);
  digitalWrite(windPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(windPin), addcount, CHANGE);

  // Inisialisasi buzzer
  pinMode(buzzerPin, OUTPUT);

  // Inisialisasi BMP280
  if (!bmp.begin(0x76)) {
    while (1);
  }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);

  // Inisialisasi TSL2561
  if (!tsl.begin()) {
    while (1);
  }
  tsl.enableAutoRange(true); // Sesuaikan rentang secara otomatis
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS); // Waktu integrasi pendek untuk pembacaan cepat
  
  // Inisialisasi LED
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Matikan LED saat mulai
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Baca suhu dan kelembapan dari DHT
    suhu = dht.readTemperature();
    kelembapan = dht.readHumidity();

    // Hitung kecepatan angin dari anemometer
    windvelocity();
    RPMcalc();
    WindSpeed();

    // Baca tekanan dari BMP280
    float pressure = bmp.readPressure();

    // Baca intensitas cahaya dari TSL2561
    sensors_event_t event;
    tsl.getEvent(&event);
    lux = event.light;

    // Debugging untuk memeriksa nilai sensor
    Serial.print("Suhu: ");
    Serial.println(suhu);
    Serial.print("Kelembapan: ");
    Serial.println(kelembapan);
    Serial.print("Kecepatan Angin: ");
    Serial.print(kalibrasi);
    Serial.println(" [m/s]");
    Serial.print("Tekanan:  ");
    Serial.println(pressure);
    Serial.print("Lux: ");
    Serial.println(lux);

    // Algoritma penentuan kondisi cuaca
    String kondisiCuaca;
    if (isnan(lux) || lux == 0) { // Jika lux tidak valid atau nol
      kondisiCuaca = "DALAM RUANGAN";
    } else if (suhu >= 20 && suhu <= 35 && kalibrasi < 5 && lux > 5000) {
      kondisiCuaca = "CERAH";
    } else if (suhu > 20 && suhu <= 35 && kalibrasi > 5 && lux > 5000) {
      kondisiCuaca = "CERAH BERANGIN";
    } else if (suhu >= 20 && suhu <= 35 && kalibrasi < 5 && lux >= 1000 && lux <= 5000) {
      kondisiCuaca = "MENDUNG";
    } else if (suhu >= 20 && suhu <= 35 && kalibrasi >= 5 && kalibrasi <= 12 && lux >= 1000 && lux <= 5000) {
      kondisiCuaca = "MENDUNG BERANGIN";
    } else if (suhu >= 20 && suhu <= 30 && kalibrasi > 12 && lux >= 1000 && lux <= 5000) {
      kondisiCuaca = "CUACA BURUK";
    } else {
      kondisiCuaca = "DALAM RUANGAN";
    }

    // Dapatkan waktu sekarang dari RTC
    DateTime now = rtc.now();
    String waktuStr = String(daysOfTheWeek[now.dayOfTheWeek()]) + " " + 
                      String(now.hour()) + ":" + 
                      (now.minute() < 10 ? "0" : "") + 
                      String(now.minute()) + " WIB";

    // Tampilkan nilai sensor di Serial Monitor beserta waktu
    if (!isnan(suhu) && !isnan(kelembapan) && !isnan(kalibrasi)) {
      Serial.println("Waktu: " + waktuStr);
      Serial.print("Kondisi Cuaca: ");
      Serial.println(kondisiCuaca);
      Serial.println(); // Baris kosong untuk pemisah
    }

    // Format data sensor untuk dikirim ke ThingSpeak melalui ESP8266
    String sensorData = "field1=" + String(suhu, 2) + "&field2=" + String(kelembapan, 2) + 
                        "&field3=" + String(kalibrasi) + "&field4=" + String(pressure, 2) +
                        "&field5=" + String(lux);

    // Debugging: Tampilkan data sensor yang akan dikirim
    Serial.println("Sending data: " + sensorData);

    // Kirim data sensor ke Serial1 (untuk ESP8266)
    Serial1.print("AT+CIPSEND="); 
    Serial1.println(sensorData.length() + 2); 
    delay(100); // Tunggu
    Serial1.print(sensorData); 
    Serial1.print("\r\n"); 

    // Cek jika kondisi cuaca berubah, bunyikan buzzer
    if (kondisiCuaca != previousKondisiCuaca) {
      digitalWrite(buzzerPin, HIGH);
      delay(500);
      digitalWrite(buzzerPin, LOW);
      previousKondisiCuaca = kondisiCuaca;
      
      // Update tampilan LCD
      lcd.clear();
      lcd.setCursor((16 - kondisiCuaca.length()) / 2, 0); 
      lcd.print(kondisiCuaca);
      lcd.setCursor((16 - waktuStr.length()) / 2, 1); 
      lcd.print(waktuStr);
    } else if (waktuStr != previousWaktuStr) {
      // Update waktu pada LCD jika hanya waktu yang berubah
      lcd.setCursor((16 - waktuStr.length()) / 2, 1); 
      lcd.print(waktuStr);

      previousWaktuStr = waktuStr;
    }
  }

  // Cek status koneksi WiFi dari ESP8266
  if (Serial1.available()) {
    String status = Serial1.readStringUntil('\n');
    if (status.indexOf("WIFI_CONNECTED") != -1) {
      wifiConnected = true;
      digitalWrite(ledPin, HIGH); // Nyalakan LED jika WiFi terhubung
    } else if (status.indexOf("WIFI_DISCONNECTED") != -1) {
      wifiConnected = false;
      digitalWrite(ledPin, LOW); // Matikan LED jika WiFi terputus
    }
  }
}

// Fungsi untuk menghitung RPM
void RPMcalc() {
  noInterrupts();
  RPM = (counter / jml_celah) * (60 * 1000 / period);
  counter = 0;
  interrupts();
}

// Fungsi untuk menghitung kecepatan angin
void WindSpeed() {
  float circum = (2 * pi * radio) / 1000; // circumference in meters
  kalibrasi = circum * RPM / 60; // wind speed in m/s
}

// Fungsi untuk menghitung kecepatan angin dengan jeda
void windvelocity() {
  delay(period);
  RPMcalc();
  WindSpeed();
}
