// Hello World example
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

void setup() {
  ds::System::begin();
  ds::System::log->println("Hello, world!");
}

void loop() {
}
