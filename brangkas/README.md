program brangkas cerdas berbasis arduino uno dan sistem keamanan fingerprint
modul:

1. arduino uno
2. sensor fingerprint R307
3. lock door selenoid
4. DFPlayer
5. buzzer 5v ky-012
6. push button
7. speaker
8. led : hijau dan merah
9. lcd 16x2
10. relay
11. sensor getar sw420

cara kerja:
1. menggunakan baterai untuk mengalirkan daya dan mengaktifkan alat.
2. sensor sidik jadi berfungsi untuk mendeteksi dan mengenali sidik jari untuk membuka pintu
3. dfplayer berfungsi untuk menampung slot kartu SD yang berfungsi untuk menyimpan file MP3 yang digunakan untuk output suara melalui speaker
4. jika sensor sidik jari mendeteksi pola yang benar maka led hijau akan menyala, dilanjutkan speaker yang mengeluarkan bunyi "kunci terbuka", lalu lcd akan menampilkan tulisan "kunci terbuka" dan kunci lock door selenoid akan terbuka.
5. jika sensor sidik jari mendeteksi pola yang salah maka led merah akan menyala, dilanjutkan speaker yang berbunyi "sidik jari salah", lcd akan menampilkan tulisan "sidik jari salah" dan lock door selenoid tetap terkunci
6. jika brangkas mendapat guncangan atau percobaan membuka pintu secara paksa maka sensor getar sw420 akan menerima input dan akan meneruskannya ke buzzer yang akan berbunyi selama beberapa detik sebagai output alarm peringatan.
7. push button berfungsi untuk mengunci kembali door lock selenoid jika sebelumnya kunci telah dibuka.
8. 
