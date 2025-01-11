/***************** 
 *  Konfigurasi pengkabelan Fingerprint module ke Arduino 
 *   Kabel Merah 5v 
 *   Kabel Hitam GND
 *   Kabel Kuning(TX) D2
 *   Kabel Hijau(RX) D3
 *   
 *   ****************
 *   Konfigurasi Pengkabelan LCD I2C ke Arduino
 *   Kabel Orange VCC 5V
 *   Kabel Coklat GND
 *   Kabel Kuning (SDA) A4
 *   Kabel Putih (9SCL) A5
 *   
 *   *****************
 *   Konfigurasi  Pengkabelan DF3PLAYER ke Arduino
 *   kabel Merah 5v
 *   Kabel Hitam GND
 *   Kabel biru (RX) D11
 *   Kabel Ungu (TX) D10
 *   
 *   
 ******************/

#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <DFPlayer_Mini_Mp3.h>
#include <SoftwareSerial.h>

#define lock 12

const int Sensorgetar = 9;
const int Buzzer = 8;
const int LedHijau=7;
const int LedMerah=6;

int button=5;
int PinGetar = 0;

unsigned long lastVibrationTime = 0;
unsigned long vibrationDuration = 3000;

int keamanan = 0;


SoftwareSerial fingerSerial(2, 3); //TX = D2, RX= D3 
SoftwareSerial dfpSerial(10, 11); //RX=10 , TX=11 
LiquidCrystal_I2C lcd(0x27, 16, 2);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

void setup()  
{

  lcd.begin();
  pinMode (lock,OUTPUT);
  digitalWrite(lock,HIGH);
  Serial.begin(9600);
  pinMode (Sensorgetar, INPUT_PULLUP);
  pinMode (Buzzer, OUTPUT);
  pinMode (LedHijau, OUTPUT);
  pinMode (LedMerah, OUTPUT);
  pinMode (button, INPUT);
  
  fingerSerial.begin(9600);
  dfpSerial.begin(9600);
  
  mp3_set_serial(dfpSerial);
  delay(10);

  mp3_set_volume (50);
  delay(10);
  
  while (!Serial);  
  delay(100);
  Serial.println("Tunggu Sebentar ");

  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Fingerprint siap digunakan");    
    lcd.setCursor(2, 0);
    lcd.print("FINGERPRINT");
    lcd.setCursor(1, 1);
    lcd.print("SIAP DIGUNAKAN");
    mp3_play (3);
    delay(3000);
    lcd.clear();
  } else {
    Serial.println("Fingerprint eror :( ");
    lcd.setCursor(0, 0);
    lcd.print("TIDAK TERDETEKSI");
    Serial.println("Coba cek pengkabelan ");
    lcd.setCursor(1, 1);
    lcd.print("COBA CEK KABEL");
    delay(3000);
    lcd.clear();
    while (1) { delay(1); }
  }

  finger.getTemplateCount();
  Serial.print("Terdapat "); Serial.print(finger.templateCount); Serial.println(" sidik jari yang terdaftar");
  Serial.println("Silahkan tempelkan jarimu");
   
   lcd.clear();
   lcd.setCursor(4, 0);
   lcd.print("Silahkan");
   lcd.setCursor(0, 1);
   lcd.print("Tempelkan Jarimu");
   mp3_play(4);
   delay(3000);
}

void loop()                     
{kembali:
  getFingerprintIDez();
  delay(50);       
  PinGetar = digitalRead(Sensorgetar); 


        if(PinGetar ==HIGH){
          // Simpan waktu deteksi getaran
        if (lastVibrationTime == 0) {
        lastVibrationTime = millis();
         }

        // Periksa apakah sudah mencapai durasi yang diinginkan
        if (millis() - lastVibrationTime >= vibrationDuration) {
          keamanan=1;
          }
        }

        while(keamanan ==1)
        {
          digitalWrite(LedHijau,LOW);
          PinGetar = digitalRead(Sensorgetar);
          Serial.println("PROTOKOL KEAMANAN");
          digitalWrite(LedMerah, HIGH);
          lcd.clear();
          lcd.setCursor(4, 0);
          lcd.print("PROTOKOL");
          lcd.setCursor(4, 1);
          lcd.print("KEAMANAN");         
          mp3_play(7);
          delay(6000);
          digitalWrite(Buzzer,HIGH);
          digitalWrite(lock,HIGH);
          delay(10000);
          digitalWrite(Buzzer,LOW);
          lcd.clear();

          {
            keamanan=0;
            goto kembali;
          }
        }

   if (digitalRead(button)==HIGH){
    digitalWrite(LedHijau,LOW);
    Serial.println("PINTU TERKUNCI");
    digitalWrite(LedMerah,HIGH);
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("PINTU");
    lcd.setCursor(4, 1);
    lcd.print("TERKUNCI");
    mp3_play(8);
    digitalWrite(lock,HIGH);
    delay(2000);
    digitalWrite(LedMerah,LOW);
   }
        
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Sidik Jari OK");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tidak Ada Sidik");
      lcd.setCursor(0, 1);
      lcd.print("jari terdeteksi");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("Kesalahan");
      lcd.setCursor(3, 1);
      lcd.print("Komunikasi");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Gagal Membaca");
      lcd.setCursor(3, 1);
      lcd.print("Sidik jari");
      return p;
    default:
      Serial.println("Unknown error");
      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.print("Gagal");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Sidik Jari OK");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("Sidik Jari");
      lcd.setCursor(2, 1);
      lcd.print("Kurang Jelas");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("Kesalahan");
      lcd.setCursor(3, 1);
      lcd.print("Komunikasi");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Tidak dapat");
      lcd.setCursor(2, 1);
      lcd.print("Identifikasi");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("Gambar");
      lcd.setCursor(3, 1);
      lcd.print("Tidak Valid");
      return p;
    default:
      Serial.println("Unknown error");
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("GAGAL");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 
  return finger.fingerID;
}

int getFingerprintIDez() {
  
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
// found a match!
  Serial.print("Terdeteksi jari no : "); Serial.print(finger.fingerID); 
  Serial.print(" Dengan keakuratan : "); Serial.print(finger.confidence-8); Serial.println("%"); 
  if (finger.confidence>80){
    digitalWrite(LedMerah, LOW);
    digitalWrite(LedHijau, HIGH);
    lcd.clear();
    Serial.println("Pintu terbuka");   
    lcd.setCursor(2, 0);
    lcd.print("PINTU TERBUKA");
    
    mp3_play (1);
    digitalWrite(lock,LOW);
    delay(2000);
    //lcd.clear();
  }
     
      else {
    digitalWrite(LedHijau, LOW);
    digitalWrite(LedMerah, HIGH);
    digitalWrite(lock,HIGH);
    Serial.println("Sidik Jari Kurang jelas");
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("SIDIK JARI");
    lcd.setCursor(2, 1);
    lcd.print("KURANG TEPAT");
    
    mp3_play(5);
    delay(2300);
    
    
    lcd.clear();
  } 
  return finger.fingerID; 
}
