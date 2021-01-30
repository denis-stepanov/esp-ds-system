// Time actions example
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

#include <sys/time.h>    //  This header is needed to simulate a time sync. Not needed for time actions

using namespace ds;

time_t old_time, new_time;

void setup() {
  System::begin();

  // Set system time to some value
  new_time = 1000000000;                              // Some date around 2001
  old_time = new_time;
  struct timeval tv_new_time = {new_time, 0};
  settimeofday(&tv_new_time, NULL);
  delay(100);                                         // Allow new time to propagate

  // Set up two timers one minute apart
  for (uint8_t n = 1; n <= 2; n++) {
    new_time += 60;
    struct tm *tinfo = localtime(&new_time);
    Timer my_timer(tinfo->tm_hour, tinfo->tm_min);
    System::timers.push_front(my_timer);
    System::log->printf(TIMED("Timer %hhu set to fire every day at %02d:%02d\n"), n, tinfo->tm_hour, tinfo->tm_min);    
  }
}

void loop() {
  new_time = System::getTime();
  if (new_time != old_time) {
    old_time = new_time;
    struct tm *tinfo = localtime(&new_time);
    System::log->printf(TIMED("Current time: %02d:%02d\n"), tinfo->tm_hour, tinfo->tm_min);
  }
  System::update();
}
