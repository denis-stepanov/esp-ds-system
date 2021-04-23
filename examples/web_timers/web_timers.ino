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
  if (timer->getAction() == "lamp on") {

    // Turn the lamp on here
    // ...
    lamp_on = true;
    System::log->println("Lamp is ON");
  } else
  if (timer->getAction() == "lamp off") {

    // Turn the lamp off here
    // ...
    lamp_on = false;
    System::log->println("Lamp is OFF");
  } else
  // Example of using an icon in timer action
  // Unicode 'ELECTRIC LIGHT BULB' (U+1f4a1) == UTF-8 'f0.9f.92.a1' == UTF-16 'd83d.dca1'
  if (timer->getAction() == "\xf0\x9f\x92\xa1 lamp toggle") {       // Here use UTF-8

    // Toggle the lamp here
    // ...
    lamp_on = !lamp_on;
    System::log->printf("Lamp is %s\n", lamp_on ? "ON" : "OFF");
  }
}
void (*System::timerHandler)(const TimerAbsolute*) = myTimerHandler;  // Install the handler

void setup() {
  System::begin();

  // Register some timer actions (used for action drop-down selector on the web)
  System::timer_actions.push_front("lamp off");
  System::timer_actions.push_front("lamp on");
  System::timer_actions.push_front("\\ud83d\\udca1 lamp toggle");  // Here use UTF-16 and escape it to pass as-is to JavaScript
  System::timer_actions.reverse();     // Optional; call this to present the order as written

  System::log->printf("Open http://%s/timers in web browser and add some timers\n", System::getLocalIPAddress().c_str());
}

void loop() {
  System::update();
}
