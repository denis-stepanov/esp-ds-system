// Example of time synchronization from NTP
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

// Set network parameters
const char *System::wifi_ssid = "mySSID";             // Network SSID
const char *System::wifi_pass = "myPassword";         // Network password

// NTP server defaults to pool.ntp.org; it can be overridden in the sketch as follows
// const char *System::time_server = "fr.pool.ntp.org";

void printTime() {
  System::log->printf(TIMED("Current time: "));
  System::log->print(System::getTimeStr());
  System::log->print(", sync status: ");
  switch (System::getTimeSyncStatus()) {
    case TIME_SYNC_NONE:     System::log->println("not synchronized"); break;
    case TIME_SYNC_OK:       System::log->println("synchronized"); break;
    case TIME_SYNC_DEGRADED: System::log->println("degraded"); break;
    default:                 System::log->println("unknown");
  }
}

// If you need custom reaction on time sync events, set up a hook as follows. This is optional
void myTimeSyncHook() {
  if (System::getTimeSyncStatus() == TIME_SYNC_OK) {
    System::log->printf(TIMED("Time has been synchronized from "));
    System::log->println(System::getTimeServer());
  }
}
void (*System::onTimeSync)() = myTimeSyncHook;

void setup() {
  System::begin();
}

void loop() {
  if (System::newSecond())
    printTime();
  System::update();
}
