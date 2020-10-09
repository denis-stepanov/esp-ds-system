// Empty sketch (analog of BareMinimum sketch in Arduino). The purpose is mostly to test compilation
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

void setup() {
  System::begin();
}

void loop() {
  System::update();
}
