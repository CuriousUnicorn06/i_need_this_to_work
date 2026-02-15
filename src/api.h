#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <Arduino.h>
#include <WebServer.h>
extern WebServer server;
extern String habitName;
extern bool personA_done;
extern bool personB_done;
void handleStatus() {
  String json = "{";
  json += "\"habit\":\"" + habitName + "\",";
  json += "\"personA_done\":" + String(personA_done ? "true" : "false") + ",";
  json += "\"personB_done\":" + String(personB_done ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}