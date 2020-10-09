// Builtin LED example ("blink" equivalent)
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

void setup() {
  System::begin();
  System::led.Blink(1000, 2000).Forever();
}

void loop() {
  System::update();
}
