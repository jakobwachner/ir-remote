#include "web.h"

const char* ssid = "Einfache-Kiste";
const char* password = "Familiewachner3!";

WebServer server(80);

#define RECV_PIN 14
#define SEND_PIN 13

IRrecv irrecv(RECV_PIN);
IRsend irsend(SEND_PIN);

decode_results results;


DynamicJsonDocument lastDoc(2048);

void storeCommand(decode_results* results) {
  lastDoc.clear();

  lastDoc["protocol"] = typeToString(results->decode_type);
  lastDoc["value"] = (uint32_t)results->value;
  lastDoc["address"] = results->address;
  lastDoc["command"] = results->command;
  lastDoc["bits"] = results->bits;

  uint16_t copyLen = results->rawlen;
  if (copyLen > 0) copyLen -= 1;
  if (copyLen > 100) copyLen = 100;
  lastDoc["rawLen"] = copyLen;

  JsonArray raw = lastDoc.createNestedArray("raw");
  for (int i = 0; i < copyLen; i++) {
    raw.add((uint16_t)(results->rawbuf[i + kStartOffset] * kRawTick));
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(50);

  irrecv.enableIRIn();
  irsend.begin();

  Serial.println("bereit");

  WiFi.begin(ssid, password);
  Serial.print("Verbinde mit WLAN");
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print('.');
  }
  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  setupWeb(server);
  server.begin();
}

void loop() {
  if (irrecv.decode(&results)) {
    storeCommand(&results);
    irrecv.resume();
  }

  server.handleClient();
  delay(1);
}
