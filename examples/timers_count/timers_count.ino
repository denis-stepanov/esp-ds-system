// Countdown timer example
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

#include <sys/time.h>    // This header is needed to simulate a time sync. Not needed for time actions
#include <TZ.h>          // Same, this is just for a sake of example. Should no longer be required after fix https://github.com/denis-stepanov/esp-ds-system/issues/23

using namespace ds;

// Timer handler
void myTimerHandler(const TimerAbsolute& timer) {
  static bool lamp_is_on = false;
  
  if (timer.getLabel() == "lamp toggle") {
    if (!lamp_is_on) {

      // Turn the lamp on here
      // ...
      System::log->println("Lamp is ON");
    } else {
      
     // Turn the lamp off here
     // ...
     System::log->println("Lamp is OFF");
    }
    lamp_is_on = !lamp_is_on;
  }
}
void (*System::timerHandler)(const TimerAbsolute&) = myTimerHandler;  // Install the handler

time_t old_time, new_time;

void set_clock(const time_t _new_time) {
  setenv("TZ", String(DS_TIMEZONE).c_str(), 1);       // String() trick is needed because setenv() would not accept a string from flash
  tzset();
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

  // Set up a countdown timer with 5 seconds period
  TimerCountdown my_timer("lamp toggle", 5);
  System::timers.push_front(my_timer);
  System::log->printf("Timer \"%s\" set to fire every %u s counting from %02hhuh%02hhum\n",
    my_timer.getLabel().c_str(), my_timer.getInterval(), my_timer.getRefHour(), my_timer.getRefMinute());
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
