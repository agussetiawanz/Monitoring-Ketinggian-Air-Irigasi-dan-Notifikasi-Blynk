#define BLYNK_TEMPLATE_ID "TMPL68PRKxKVz"
#define BLYNK_TEMPLATE_NAME "Monitoring Ketinggian Air"
#define BLYNK_AUTH_TOKEN "rA7Iqd9GqTQDVP-1PG1iBXtLf9Lgkip4"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

char ssid[] = "Wokwi-GUEST"; // WiFi simulasi Wokwi
char pass[] = "";            // Kosongkan password

#define TRIG_PIN 26
#define ECHO_PIN 25

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD I2C 16x2

float tinggi_max = 100.0; // cm (ubah sesuai tinggi bak air sebenarnya)
float duration_us, distance_cm;

BlynkTimer timer;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Monitoring Air");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L, kirimData); // Setiap 1 detik
}

void loop() {
  Blynk.run();
  timer.run();
}

void kirimData() {
  // Kirim pulsa trigger 10us
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Hitung waktu pantulan dan konversi ke cm
  duration_us = pulseIn(ECHO_PIN, HIGH);
  distance_cm = duration_us * 0.034 / 2;

  // Hitung tinggi air dari dasar
  float tinggi_air = tinggi_max - distance_cm;
  if (tinggi_air < 0) tinggi_air = 0;

  // Tampilkan ke LCD
  lcd.setCursor(0, 1);
  lcd.print("Tinggi: ");
  lcd.print(tinggi_air, 1); // 1 angka desimal
  lcd.print(" cm   ");      // Hapus karakter sisa

  // Kirim ke Blynk (Virtual Pin V0)
  Blynk.virtualWrite(V0, tinggi_air);

  // Tambahkan debug ke Serial Monitor
  Serial.print("Jarak: ");
  Serial.print(distance_cm);
  Serial.print(" cm | Tinggi air: ");
  Serial.println(tinggi_air);

  // Notifikasi jika air rendah
  if (tinggi_air < 20.0) {
    Blynk.logEvent("air_rendah", "Tinggi air di bawah 20 cm!");
  }

  delay(100); // beri waktu LCD update
}
