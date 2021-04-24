// Countdown timer example, using Ticker
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

// Timer handler for lamp toggle
void myTimerHandler_lamp_toggle() {
  static auto lamp_on = false;
  if (lamp_on) {

    // Turn the lamp off here
    // ...
    System::log->println("Lamp is OFF");
  } else {
    
    // Turn the lamp on here
    // ...
    System::log->println("Lamp is ON");

  }
  lamp_on = !lamp_on;
}

// Service timer to print a time beat
TimerCountdownTick my_timer_print_time("metronome", 1, [](){ System::log->printf(TIMED("\n") / 1000); });

// Countdown timer to flip a lamp with 5 seconds period
TimerCountdownTick my_timer_lamp_toggle("lamp toggle", 5, myTimerHandler_lamp_toggle);

void setup() {
  System::begin();
  System::log->printf("Timer \"%s\" set to fire every %.0f s\n", my_timer_lamp_toggle.getAction().c_str(), my_timer_lamp_toggle.getInterval());
}

void loop() {
}
