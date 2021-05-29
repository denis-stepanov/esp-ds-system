// Time routines support example
// For automatic time set up from NTP, see "time_ntp" example
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

void printTime() {
  System::log->printf("Current time (seconds since epoch): %lu\n", System::getTime());
  System::log->print("Current time (string): ");
  System::log->println(System::getTimeStr());
  System::log->print("Time sync status: ");
  switch (System::getTimeSyncStatus()) {
    case TIME_SYNC_NONE:     System::log->println("not synchronized"); break;
    case TIME_SYNC_OK:       System::log->println("synchronized"); break;
    case TIME_SYNC_DEGRADED: System::log->println("degraded"); break;
    default:                 System::log->println("unknown");
  }
}

// If you need to react on time sync events, set up a hook as follows
void myTimeSyncHook() {
  System::log->println("User code caught time sync event");

  // Can do something useful here...
}
void (*System::onTimeSync)() = myTimeSyncHook;

void setup() {
  System::begin();

  // On boot, time is set to the beginning of epoch
  printTime();

  // Set system time from an external source (e.g., RTC clock), causing it to become synchronized
  const time_t new_time = 1000000000;                 // Some date around 2001
  System::log->print("Time will be set to: ");
  System::log->println(System::getTimeStr(new_time)); // Illustrates getTimeStr() with a parameter
  System::setTime(new_time);
  delay(100);                                         // Allow new time to propagate
  printTime();
}

void loop() {
}
