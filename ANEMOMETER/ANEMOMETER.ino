#include <Wire.h> 

volatile byte half_revolutions; // variabel tipe data byte
unsigned int rpmku; // variabel tipe data integer
unsigned long timeold; // variabel tipe data long

int kalibrasi; // variabel tipe data integer

void setup() {
  attachInterrupt(0, rpm_fun, RISING); // mengambil sinyal high pada pin 2
  half_revolutions = 0; // memberikan nilai 0 pada variabel
  rpmku = 0;
  timeold = 0;
  kalibrasi = 0;

  Serial.begin(9600);
  Serial.println("Setup complete. Waiting for wind speed data...");
}

void loop() {
  rpmku = 30 * 1000 / (millis() - timeold) * half_revolutions; // menghitung rpm
  timeold = millis(); // hasil counter dimasukkan ke variabel timeold
  half_revolutions = 0; // reset variabel

  kalibrasi = (rpmku - 150) / 109;  // rumus kalibrasi
  
  if ((kalibrasi > 590) && (kalibrasi < 605)) {
    kalibrasi = 0;
  }

  Serial.print("Kecepatan angin: ");
  Serial.print(kalibrasi);
  Serial.println(" m/s");

  delay(200);
}

void rpm_fun() {
  half_revolutions++; // counter interrupt
}
