// Uptime reporting example
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

void setup() {
  System::begin();
}

void loop() {
  System::log->printf(TIMED("Uptime: "));
  System::log->println(System::getUptimeStr());
  delay(1000);

  // There is no strict need to call System::update() in this case
}
