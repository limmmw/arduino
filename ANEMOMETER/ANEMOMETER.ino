#include <Wire.h> 

volatile byte half_revolutions; 
unsigned int rpmku; 
unsigned long timeold; 

int kalibrasi; 

void setup() {
  attachInterrupt(0, rpm_fun, RISING); 
  half_revolutions = 0; 
  rpmku = 0;
  timeold = 0;
  kalibrasi = 0;

  Serial.begin(9600);
  Serial.println("Setup complete. Waiting for wind speed data...");
}

void loop() {
  rpmku = 30 * 1000 / (millis() - timeold) * half_revolutions; 
  timeold = millis(); 
  half_revolutions = 0; 

  kalibrasi = (rpmku - 150) / 109;
  
  if ((kalibrasi > 590) && (kalibrasi < 605)) {
    kalibrasi = 0;
  }

  Serial.print("Kecepatan angin: ");
  Serial.print(kalibrasi);
  Serial.println(" m/s");

  delay(200);
}

void rpm_fun() {
  half_revolutions++;
}
