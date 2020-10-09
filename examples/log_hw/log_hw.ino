// System log example (hardware serial option)
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

void setup() {
  System::begin();
  System::log->println("Simlpe log message");
  System::log->printf("Complex %s message including number: %d\n", "log", 12345);
  System::log->printf(TIMED("Log message with millis() timestamp\n"));   // Requires printf()
}

void loop() {
}
