// Time actions example
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

#include <sys/time.h>    //  This header is needed to simulate a time sync. Not needed if time is only read

using namespace ds;

void setup() {
  System::begin();

  // Set system time to some value
  time_t new_time = 1000000000;                       // Some date around 2001
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

  System::log->println("Now sit back and wait...");
}

void loop() {
  System::update();
}
