#include <stdio.h>
#include <string.h>
#ifndef COMMS_H
#define COMMS_H

#include <WiFiUdp.h>
#include "state.h"  // Make sure this includes personB_done and LED pin

#ifndef LED_PIN
#define LED_PIN 7  // Define LED_PIN if not already defined in state.h
#endif

static WiFiUDP udp;
const unsigned int localUdpPort = 4210;         // Port to listen on
const char* photonIP = "172.20.10.13";         // Replace with your Photon’s IP
const unsigned int photonPort = 4210;           // Same port on Photon

char incomingPacket[255];  // buffer for incoming packets

void setupUDP() {
  udp.begin(localUdpPort);
  Serial.print("UDP listening on port ");
  Serial.println(localUdpPort);
}

void sendUDPMessage(const char* message) {
  udp.beginPacket(photonIP, photonPort);
  udp.write((const uint8_t*)message, strlen(message));  // Cast + length
  udp.endPacket();
  Serial.print("Sent UDP: ");
  Serial.println(message);
}

void checkForIncomingUDP() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;
    }
    Serial.print("Received UDP: ");
    Serial.println(incomingPacket);

    // Handle known messages
    if (strcmp(incomingPacket, "B_DONE") == 0) {
      personB_done = true;
      digitalWrite(LED_PIN, HIGH);  // Optional: light up LED for feedback
    }
  }
}

#endif