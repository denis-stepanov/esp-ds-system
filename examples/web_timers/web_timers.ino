// Example of timer configuration via web
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

// Set network parameters
const char *System::wifi_ssid = "mySSID";             // Network SSID
const char *System::wifi_pass = "myPassword";         // Network password

void setup() {
  System::begin();

  // Define some timer actions
  System::timer_actions.push_front("lamp off");
  System::timer_actions.push_front("lamp on");
  System::timer_actions.push_front("\\uD83D\\uDCA1 lamp toggle");  // Example of action including an icon (must be UTF-16 encoded for use with JS)
  System::timer_actions.reverse();     // Optional; call this to keep the order as written

  // Add a timer at 07:50 every Sunday to turn the lamp off
  auto my_timer = new TimerAbsolute("lamp off", 7, 50, 0, TIMER_DOW_SUNDAY);
  System::timers.push_front(my_timer);

  // Add a timer at quarter to sunset to turn the lamp on
  my_timer = new TimerSolar(TIMER_SUNSET, "lamp on", -15);
  System::timers.push_front(my_timer);

  // Add a timer firing every hour offset from midnight by 30 mins. Disable it by default
  my_timer = new TimerCountdownAbs("lamp toggle", 60, 30, TIMER_DOW_ANY, 0);
  System::timers.push_front(my_timer);

  System::timers.reverse();     // Optional; call this to keep the order as written
  System::log->printf("Open http://%s/timers in web browser\n", System::getLocalIPAddress().c_str());
}

void loop() {
  System::update();
}
