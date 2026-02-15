#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <vector>

using namespace std;
#define BUTTON_PIN 21
#define PIXEL_PIN 18
vector<String> tasksList;
volatile int completed = 0;
Adafruit_NeoPixel pixel(1, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

int totaltasks = 0;
int pressCount = 0;
int change = 0;
int red = 255;
int green = 0;

bool lastState = HIGH;

const char* ssid     = "Avery's S22";
const char* password = "dvtqwb8b8d2d6nm";

WebServer server(80);


String makePage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>Tasks</title>
  <style>
    body{font-family:system-ui;margin:20px;}
    textarea{width:100%;max-width:520px;height:140px;}
    .wrap{max-width:520px;}
    li{margin:8px 0;}
  </style>
</head>
<body>
<div class="wrap">
  <h2>Enter tasks (one per line)</h2>
  <form action="/setTasks" method="POST">
    <textarea name="tasks" placeholder="- Homework&#10;- Gym&#10;- Laundry"></textarea><br><br>
    <button type="submit">Save Tasks</button>
  </form>

  <br><br>
  <form action="/reset" method="POST">
    <button type="submit">Reset</button>
  </form>

  <h3>To-do</h3>
  <ul id="list"></ul>

  <p><b>Completed:</b> <span id="done">0</span>/<span id="total">0</span></p>
</div>

<script>
async function refresh(){
  const res = await fetch('/status');
  const data = await res.json();

  document.getElementById('done').textContent = data.completed;
  document.getElementById('total').textContent = data.total;

  const list = document.getElementById('list');
  list.innerHTML = '';

  for (let i=0; i<data.tasks.length; i++){
    const li = document.createElement('li');
    const cb = document.createElement('input');
    cb.type = 'checkbox';
    cb.checked = (i < data.completed);
    cb.disabled = true;
    li.appendChild(cb);
    li.appendChild(document.createTextNode(' ' + data.tasks[i]));
    list.appendChild(li);
  }
}

refresh();
setInterval(refresh, 300); // update 3x/second
</script>
</body>
</html>
)rawliteral";
  return html;
}


void handleRoot() {
  server.send(200, "text/html", makePage());
}
void handleReset() {
  tasksList.clear();
  completed = 0;
  totaltasks = 0;
  pressCount = 0;

  red = 255;
  green = 0;

  pixel.setPixelColor(0, pixel.Color(0,0,0));
  pixel.show();

  server.sendHeader("Location", "/");
  server.send(303);
}
void handleSetTasks() {
  String raw = server.arg("tasks");

  tasksList.clear();
  completed = 0;

  
  raw.replace("\r", "");
  int start = 0;
  while (true) {
    int nl = raw.indexOf('\n', start);
    String line = (nl == -1) ? raw.substring(start) : raw.substring(start, nl);
    line.trim();
    if (line.length() > 0) tasksList.push_back(line);
    if (nl == -1) break;
    start = nl + 1;
  }
  totaltasks = (int)tasksList.size();          
  pixel.setPixelColor(0, pixel.Color(0, 255, 0)); 
  pixel.show();
  server.sendHeader("Location", "/");
  server.send(303); 
}
void handleStatus() {
  int total = (int)tasksList.size();
  int done = completed;
  if (done > total) done = total;

  String json = "{";
  json += "\"total\":" + String(total) + ",";
  json += "\"completed\":" + String(done) + ",";
  json += "\"tasks\":[";
  for (int i=0; i<total; i++) {
    String t = tasksList[i];
    t.replace("\\", "\\\\"); t.replace("\"", "\\\"");
    json += "\"" + t + "\"";
    if (i != total-1) json += ",";
  }
  json += "]}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pixel.begin();
  pixel.setBrightness(50);
  pixel.setPixelColor(0, pixel.Color(0,0,0));
  pixel.show();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/setTasks", HTTP_POST, handleSetTasks);
  server.on("/reset", HTTP_POST, handleReset);
  server.on("/status", handleStatus);
  server.begin();
}


void loop() {
  server.handleClient();

  bool current = digitalRead(BUTTON_PIN);

  // only do button logic if tasks exist
  if (totaltasks > 0) {

    // detect press (HIGH → LOW)
    if (lastState == HIGH && current == LOW) {
      pressCount++;
      if (completed < (int)tasksList.size()) {
        completed++;
      }
      delay(250);
    

      
      change = 255/totaltasks;
      red -= change;
      green += change;
      if (red < 0) red = 0;
      if (green > 255) green = 255;
      pixel.setPixelColor(0, pixel.Color(green, red, 0));
      pixel.show();
      
      // finished all tasks
      if (pressCount >= totaltasks) {
        delay(500);

        pixel.setPixelColor(0, pixel.Color(0,0,0));
        pixel.show();
        for (int i = 0; i < 13; i++) {   // ~5 seconds total

          // ON (green)
          pixel.setPixelColor(0, pixel.Color(225, 0, 0));
          pixel.show();
          delay(200);

          // OFF
          pixel.setPixelColor(0, pixel.Color(0, 0, 225));
          pixel.show();
          delay(200);
        }
        pixel.setPixelColor(0, pixel.Color(0, 0, 0));
        pixel.show();
        // reset
        totaltasks = 0;
        pressCount = 0;
        red = 0;
        green = 0;
        pixel.setPixelColor(0, pixel.Color(green, red, 0));
        pixel.show();

      }

      delay(250); // debounce
    }
  }

  lastState = current;
}
