// Countdown timer example, using absolute time
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

// Timer handler
void myTimerHandler(const TimerAbsolute* timer) {
  if (timer->getAction() == "lamp on") {

    // Turn the lamp on here
    // ...
    System::log->println("Lamp is ON");
  } else
  if (timer->getAction() == "lamp off") {

    // Turn the lamp off here
    // ...
    System::log->println("Lamp is OFF");
  }
}
void (*System::timerHandler)(const TimerAbsolute*) = myTimerHandler;  // Install the handler

time_t old_time, new_time;

void set_clock(const time_t _new_time) {
  new_time = _new_time;
  old_time = new_time;
  System::setTime(new_time);
  delay(100);                                         // Allow new time to propagate
}

void setup() {
  System::begin();

  // Set system time to some date around 2001
  set_clock(1000000000);

  // Set up a countdown timer with 10 seconds period
  // Illustrates static allocation
  static TimerCountdownAbs my_timer("lamp off", 10);
  System::timers.push_front(&my_timer);
  System::log->printf("Timer \"%s\" set to fire every %.0f s with offset of %u s from midnight\n",
    my_timer.getAction().c_str(), my_timer.getInterval(), my_timer.getOffset());

  // Set up another countdown timer with 10 seconds period and 5 seconds offset
  // Illustrates dynamic allocation
  auto my_timer2 = new TimerCountdownAbs("lamp on", 10, 5);
  if (my_timer2) {
    System::timers.push_front(my_timer2);
    System::log->printf("Timer \"%s\" set to fire every %.0f s with offset of %u s from midnight\n",
      my_timer2->getAction().c_str(), my_timer2->getInterval(), my_timer2->getOffset());
  }
}

void loop() {

  // Just display current time periodically to see when the timers fire. Lamp should toggle every 5 seconds
  new_time = System::getTime();
  if (new_time != old_time) {
    old_time = new_time;
    System::log->println(System::getTimeStr());
  }

  System::update();
}
