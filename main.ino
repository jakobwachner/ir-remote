#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Wlan";
const char* password = "admin";

WebServer server(80);


void handleRoot() {
  server.send(200, "text/plain", "ESP32 server running");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  WiFi.begin(ssid, password);

  
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi verbunden!");
  Serial.print("IP addresse: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  
  server.begin();
  Serial.println("HTTP server wurde gestartet");
}

void loop() {
  server.handleClient();
}
