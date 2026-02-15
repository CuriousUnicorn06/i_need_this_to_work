#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "comms.h"
#include "state.h"    
#include "api.h"      

const char* ssid     = "alaynas iPhone";
const char* password = "abcdefghij";

WebServer server(80);

// ---- WEBPAGE ----
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>To-Do Competition</title>
  <style>
    body{
      margin:0;
      font-family: system-ui, -apple-system, Segoe UI, Roboto, Arial, sans-serif;
      background: #ffc3d5;
      color:#111;
      display:flex;
      min-height:100vh;
      align-items:center;
      justify-content:center;
      padding:20px;
    }
    .card{
      width: min(520px, 100%);
      background:#ffe4ec;
      border-radius:16px;
      box-shadow: 0 10px 30px rgba(0,0,0,0.08);
      padding:22px;
    }
    h1{
      margin:0 0 6px 0;
      font-size:22px;
    }
    .sub{
      margin:0 0 18px 0;
      color:#555;
      font-size:14px;
    }
    .row{
      display:flex;
      gap:10px;
      margin: 10px 0 16px 0;
    }
    .pill{
      flex:1;
      border:1px solid #ddd;
      border-radius:12px;
      padding:12px 10px;
      display:flex;
      gap:10px;
      align-items:center;
      cursor:pointer;
      user-select:none;
    }
    .pill input{ accent-color:#111; }
    label span{ font-weight:600; }
    textarea{
      width:100%;
      box-sizing:border-box;
      border:1px solid #ddd;
      border-radius:12px;
      padding:12px;
      font-size:14px;
      line-height:1.4;
      resize:vertical;
      min-height:140px;
      outline:none;
    }
    textarea:focus{
      border-color:#111;
      box-shadow:0 0 0 3px rgba(0,0,0,0.08);
    }
    .btn{
      width:100%;
      border:none;
      border-radius:12px;
      padding:12px 14px;
      font-size:15px;
      font-weight:700;
      cursor:pointer;
      background:ffe4ec;
      color:#fff;
      margin-top:14px;
    }
    .btn:active{ transform: translateY(1px); }
    .hint{
      margin-top:10px;
      font-size:12px;
      color:#666;
    }
  </style>
</head>

<body>
  <div class="card">
    <h1>To-Do Competition</h1>
    <p class="sub">Pick a user, paste your list, hit save.</p>

    <form action="/submit" method="GET">
      <div class="row">
        <label class="pill">
          <input type="radio" name="user" value="User1" required>
          <span>User 1</span>
        </label>

        <label class="pill">
          <input type="radio" name="user" value="User2">
          <span>User 2</span>
        </label>
      </div>

      <textarea name="todo" placeholder="- example&#10"></textarea>

      <button class="btn" type="submit">Save List</button>
      <div class="hint">Tip: each line becomes one task later.</div>
    </form>
  </div>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleSubmit() {
  String user = server.arg("user");
  String todo = server.arg("todo");

  Serial.println("----- NEW LIST -----");
  Serial.println("User: " + user);
  Serial.println("To-do:\n" + todo);

  String response = "<h2>Saved for " + user + "!</h2>";
  response += "<pre>" + todo + "</pre>";
  response += "<a href='/'>Go back</a>";

  server.send(200, "text/html", response);
}

void setup() {
   Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected");
  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);
  server.on("/submit", handleSubmit);
  server.begin();
  
  setupUDP();
  Serial.println("Sending test packet...");
  sendUDPMessage("B_DONE");
}

void loop() {
  server.handleClient();
  
  delay(2); // ✅ tiny yield to keep system happy
}