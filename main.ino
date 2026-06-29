#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Wlan";
const char* password = "admin";

const int IR_PIN = 13;
WebServer server(80);


bool ledState = false;

const char INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head><meta charset="utf-8"><title>ESP32 IR</title></head>
<body style="font-family:Arial;text-align:center;margin-top:50px;">
<h2>ESP32 IR</h2>
<button style="padding:20px;font-size:20px;" onclick="toggleLed()">LED umschalten</button>

<!-- fct zum umschalten -->
<script>
function toggleLed(){
  fetch('/toggle').then(r => r.text()).then(console.log);
}
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

void handleToggle() {
  ledState = !ledState;
  digitalWrite(IR_PIN, ledState);
  server.send(200, "text/plain", ledState ? "ON" : "OFF");
}

void setup() {
  Serial.begin(115200);
  pinMode(IR_PIN, OUTPUT);
  digitalWrite(IR_PIN, LOW);

  Serial.print("Verbinde mit WLAN");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.begin();
}

void loop() {
  server.handleClient();
}
