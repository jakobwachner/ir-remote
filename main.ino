#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Einfache-Kiste";
const char* password = "Familiewachner3!";

const int IR_LED_PIN = 13;
const int IR_RECEIVER_PIN = 14;

WebServer server(80);

bool ledState = false;
int lastReceiverState = HIGH;

const char INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>ESP32 IR Test</title>
</head>

<body style="font-family:Arial;text-align:center;margin-top:50px;">

<h2>ESP32 IR Hardware Test</h2>

<button style="padding:20px;font-size:20px;" onclick="toggleLed()">
IR LED umschalten
</button>

<h3 id="receiverState">Empfänger: --</h3>

<script>

function toggleLed(){
    fetch("/toggle");
}

function updateReceiver(){
    fetch("/receiver")
        .then(r => r.text())
        .then(text => {
            document.getElementById("receiverState").innerHTML =
                "Empfänger: " + text;
        });
}

setInterval(updateReceiver, 200);

</script>

</body>
</html>
)rawliteral";


void handleRoot() {
    server.send(200, "text/html", INDEX_HTML);
}


void handleToggle() {
    ledState = !ledState;

    digitalWrite(IR_LED_PIN, ledState);

    Serial.print("IR LED: ");
    Serial.println(ledState ? "ON" : "OFF");

    server.send(200, "text/plain", ledState ? "ON" : "OFF");
}


void handleReceiver() {

    int state = digitalRead(IR_RECEIVER_PIN);

    if(state == LOW)
        server.send(200, "text/plain", "IR erkannt");
    else
        server.send(200, "text/plain", "Kein Signal");
}


void setup() {

    Serial.begin(115200);

    pinMode(IR_LED_PIN, OUTPUT);
    digitalWrite(IR_LED_PIN, LOW);

    pinMode(IR_RECEIVER_PIN, INPUT);

    Serial.print("Verbinde mit WLAN");

    WiFi.begin(ssid, password);

    while(WiFi.status() != WL_CONNECTED){
        delay(250);
        Serial.print(".");
    }

    Serial.println();
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.on("/toggle", handleToggle);
    server.on("/receiver", handleReceiver);

    server.begin();

    Serial.println("Server gestartet.");
}


void loop() {

    server.handleClient();

    int state = digitalRead(IR_RECEIVER_PIN);

    if(state != lastReceiverState){

        lastReceiverState = state;

        if(state == LOW)
            Serial.println("IR erkannt");
        else
            Serial.println("Kein IR Signal");
    }
}
