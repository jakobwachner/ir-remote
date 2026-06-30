#include "web.h"

const char PAGE[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">

  <style>
    body {
      margin: 0;
      font-family: "Consolas", "Courier New", monospace;
      background: #1E1E1E;
      color: white;
      text-align: center;
    }

    h2 {
      margin: 20px 0;
      font-size: 22px;
    }

    .card {
      max-width: 600px;
      margin: 0 auto;
      padding: 20px;
      border-radius: 14px;
      background: #252526;
      box-shadow: 0 8px 24px rgba(0, 0, 0, 0.35);
    }

    .field {
      text-align: left;
      margin: 10px 0;
      font-size: 16px;
      line-height: 1.4;
    }

    .field strong {
      display: inline-block;
      width: 90px;
      color: #9cdcfe;
    }

    .btn {
      display: block;
      width: 90%;
      max-width: 360px;
      margin: 18px auto;
      padding: 18px;
      font-size: 20px;
      border: none;
      border-radius: 10px;
      background: #4aa3ff;
      color: white;
      cursor: pointer;
      text-decoration: none;
    }

    .btn:active {
      transform: scale(0.98);
      background: #1b4fa8;
    }

    .status {
      color: #dcdcaa;
      margin-top: 8px;
      font-size: 14px;
    }
  </style>
</head>

<body>
  <div class="card">
    <h2>ESP32 IR Monitor</h2>

    <div class="field"><strong>Protocol:</strong> <span id="protocol">-</span></div>
    <div class="field"><strong>Value:</strong> <span id="value">-</span></div>
    <div class="field"><strong>Address:</strong> <span id="address">-</span></div>
    <div class="field"><strong>Command:</strong> <span id="command">-</span></div>
    <div class="field"><strong>Bits:</strong> <span id="bits">-</span></div>
    <div class="field"><strong>RawLen:</strong> <span id="rawLen">-</span></div>
    <div class="field"><strong>Raw:</strong> <span id="raw">-</span></div>

    <button class="btn" id="sendTest">Send Test Signal</button>
    <div class="status">Last update: <span id="lastUpdate">waiting...</span></div>
  </div>

  <script>
    async function refreshLast() {
      try {
        const response = await fetch('/last');
        if (!response.ok) throw new Error('fetch failed');

        const data = await response.json();

        document.getElementById('protocol').textContent = data.protocol || '-';
        document.getElementById('value').textContent = data.value !== undefined ? data.value : '-';
        document.getElementById('address').textContent = data.address !== undefined ? data.address : '-';
        document.getElementById('command').textContent = data.command !== undefined ? data.command : '-';
        document.getElementById('bits').textContent = data.bits !== undefined ? data.bits : '-';
        document.getElementById('rawLen').textContent = data.rawLen !== undefined ? data.rawLen : '-';
        document.getElementById('raw').textContent = Array.isArray(data.raw) ? data.raw.join(', ') : '-';
        document.getElementById('lastUpdate').textContent = new Date().toLocaleTimeString();
      } catch (error) {
        console.log(error);
        document.getElementById('lastUpdate').textContent = 'error';
      }
    }

    async function sendTest() {
      await fetch('/sendTest', { method: 'POST' });
    }

    document.getElementById('sendTest').addEventListener('click', sendTest);
    refreshLast();
    setInterval(refreshLast, 1500);
  </script>
</body>
</html>
)rawliteral";

void setupWeb(WebServer& server) {
  server.on("/", [&]() {
    server.send(200, "text/html", PAGE);
  });

  server.on("/last", HTTP_GET, [&]() {
    String json;
    serializeJson(lastDoc, json);
    server.send(200, "application/json", json);
  });

  server.on("/sendTest", HTTP_POST, [&]() {
    irsend.sendNEC(0x20DF10EF, 32);
    server.send(200, "text/plain", "OK");
  });
}
