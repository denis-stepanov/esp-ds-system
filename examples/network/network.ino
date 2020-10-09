// Network example
// This will glow LED while Wi-Fi is connecting. Check MySystem.h if this is not desired
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
  if (System::networkIsConnected()) {
    System::log->printf("Network name: ");
    System::log->println(System::getNetworkName());
    System::log->print("Network details: ");
    System::log->println(System::getNetworkDetails());
    System::log->print("IP address: ");
    System::log->println(System::getLocalIPAddress());    
  } else {
    System::log->printf("Could not connect to network ");
    System::log->println(System::getNetworkName());    
  }
}

void loop() {
  // There is no strict need to call System::update() in this case
}
