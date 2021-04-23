// Example of timer configuration via web
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

// Set network parameters
const char *System::wifi_ssid = "mySSID";             // Network SSID
const char *System::wifi_pass = "myPassword";         // Network password

// Timer handler
void myTimerHandler(const TimerAbsolute* timer) {
  static auto lamp_on = false;
  if (timer->getLabel() == "lamp on") {

    // Turn the lamp on here
    // ...
    lamp_on = true;
    System::log->println("Lamp is ON");
  } else
  if (timer->getLabel() == "lamp off") {

    // Turn the lamp off here
    // ...
    lamp_on = false;
    System::log->println("Lamp is OFF");
  } else
  if (timer->getLabel() == "lamp toggle") {

    // Toggle the lamp here
    // ...
    lamp_on = !lamp_on;
    System::log->printf("Lamp is %s\n", lamp_on ? "ON" : "OFF");
  }
}
void (*System::timerHandler)(const TimerAbsolute*) = myTimerHandler;  // Install the handler

void setup() {
  System::begin();

  // Define some timer actions
  System::timer_actions.push_front("lamp off");
  System::timer_actions.push_front("lamp on");
  System::timer_actions.push_front("lamp toggle");
//  System::timer_actions.push_front("\\uD83D\\uDCA1 lamp toggle");  // Example of action including an icon (must be UTF-16 encoded for use with JS)
  System::timer_actions.reverse();     // Optional; call this to keep the order as written

  System::log->printf("Open http://%s/timers in web browser and add some timers\n", System::getLocalIPAddress().c_str());
}

void loop() {
  System::update();
}
