// Multicast DNS example
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

// Set network parameters
const char *System::hostname  = "myNodeMCU";          // My hostname
const char *System::wifi_ssid = "mySSID";             // Network SSID
const char *System::wifi_pass = "myPassword";         // Network password

void setup() {
  System::begin();
  if (System::networkIsConnected())
    System::log->printf("Ping %s.local to see if it responds\n", System::hostname);
  else
    System::log->println("Could not connect to network");
}

void loop() {
  System::update();
}
