// Boot time reporting example
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

// Set network parameters
const char *System::wifi_ssid = "mySSID";             // Network SSID
const char *System::wifi_pass = "myPassword";         // Network password

void setup() {
  System::begin();
}

void loop() {
  delay(1000);
  if (System::getTimeSyncStatus() == TIME_SYNC_OK) {
    System::log->printf(TIMED("Current time (UTC): "));
    System::log->print(System::getTimeStr());
    System::log->print(", boot time (UTC): ");
    System::log->println(System::getBootTimeStr());
  }
  System::update();
}
