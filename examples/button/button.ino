// Button example
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;
using namespace ace_button;

// Button event handler
void handleButtonEvent(AceButton* /* button */, uint8_t event_type, uint8_t /* button_state */) {
  if (event_type == AceButton::kEventPressed)
    System::log->printf(TIMED("Button pressed\n"));
}
void (*System::onButtonPress)(AceButton*, uint8_t, uint8_t) = handleButtonEvent;

void setup() {
  System::begin();
  System::log->println("Press the FLASH button");
}

void loop() {
  System::update();
}
