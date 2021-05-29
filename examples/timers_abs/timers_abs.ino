// Absolute timer example
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

#include <sys/time.h>    // This header is needed to simulate a time sync. Not needed for time actions

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
  struct timeval tv_new_time = {new_time, 0};
  settimeofday(&tv_new_time, NULL);
  delay(100);                                         // Allow new time to propagate
}

void setup() {
  System::begin();

  // Set system time to some date around 2001
  set_clock(1000000000);

  // Set up two timers one minute apart
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
  new_time = System::getTime();
  if (new_time != old_time) {
    old_time = new_time;
    System::log->println(System::getTimeStr());
  }

  System::update();
}
