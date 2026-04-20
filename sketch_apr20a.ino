#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

const char* ssid = "ESP32";
const char* password = "12345678";

// Firmware linki (.bin dosyası)
const char* firmware_url = "http://seninserver.com/firmware.bin";

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  Serial.print("Baglaniyor");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nBaglandi");
  Serial.println(WiFi.localIP());

  checkForUpdate();
}

void loop() {
  // Boş olabilir veya periyodik kontrol eklenebilir
}

void checkForUpdate() {
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;
    http.begin(firmware_url);

    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {

      int contentLength = http.getSize();
      WiFiClient * stream = http.getStreamPtr();

      if (Update.begin(contentLength)) {

        size_t written = Update.writeStream(*stream);

        if (written == contentLength) {
          Serial.println("Guncelleme tamam");
        } else {
          Serial.println("Eksik yazildi");
        }

        if (Update.end()) {
          if (Update.isFinished()) {
            Serial.println("Restart...");
            ESP.restart();
          } else {
            Serial.println("Guncelleme tamamlanmadi");
          }
        }

      } else {
        Serial.println("Yeterli alan yok");
      }

    } else {
      Serial.printf("HTTP hata: %d\n", httpCode);
    }

    http.end();
  }
}