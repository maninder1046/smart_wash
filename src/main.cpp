#include <WiFi.h>
#include "washserver.h"
#include "logic.h"

#define WIFI_SSID "maninder2.4"
#define WIFI_PASSWORD "12345678"

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
  Serial.println(WiFi.localIP());

  loadState();
  initLogic();
  initServer();
}

void loop() {
  handleServer();
  runLogic();
}
