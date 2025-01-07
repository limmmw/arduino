Stasiun cuaca berbasis mikrokontroler dan web
modul:
-Arduino Mega 2560
-NodeMCU ESP8266
-LCD 16X2
-Sensor suhu DHT-22
-Sensor Tekanan Udara BMP260
-Sensor Angin Anemometer
-Senson Cahaya TSL2561
-Buzzer
-LED

cara kerja:
1. semua sensor terhubung ke pin Arduino mega
2. nilai-nilai mentah yang di tangkap sensor di olah menggunakan arduino mega menjadi data yang matang
3. setelah nilai di olah kemudian data di kirim ke esp8266 yang yang terhubung melalui pin rx tx
4. nilai kemudian di kirimkan ke web server oleh esp8266 agar nilai nilai dari sensor dan pembacaan cuaca tampil di web
