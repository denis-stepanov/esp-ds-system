// Absolute timer example
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

void setup() {
  System::begin();

  // Set system time to some date around 2001
  System::setTime(1000000000);
  delay(100);                                         // Allow new time to propagate

  // Set up two timers one minute apart
  auto new_time = System::time;
  for (uint8_t n = 1; n <= 2; n++) {
    new_time += 60;
    struct tm *tinfo = localtime(&new_time);
    auto my_timer = new TimerAbsolute(n == 1 ? "lamp on" : "lamp off", tinfo->tm_hour, tinfo->tm_min);
    if (my_timer) {
      System::timers.push_front(my_timer);
      System::log->printf("Timer %hhu \"%s\" set to fire every day at %02hhuh%02hhum\n",
        n, my_timer->getAction().c_str(), my_timer->getHour(), my_timer->getMinute());
    }
  }
}

void loop() {

  // Just display current time periodically to see when the timer fires
  if (System::newSecond())
    System::log->println(System::getTimeStr());

  System::update();
}
