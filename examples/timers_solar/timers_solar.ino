// Example of reacting on sunruse or sunset
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

#include <sys/time.h>    //  This header is needed to simulate a time sync. Not needed for time actions
#include <TZ.h>

using namespace ds;

// Timer handler
void myTimerHandler(const TimerAbsolute& timer) {
  if (timer.getLabel() == "lamp on") {

   // Turn the lamp on here
   // ...
   System::log->println("Lamp is ON");
  } else
  if (timer.getLabel() == "lamp off") {

   // Turn the lamp off here
   // ...
   System::log->println("Lamp is OFF");
  }
}
void (*System::timerHandler)(const TimerAbsolute&) = myTimerHandler;  // Install the handler

time_t old_time, new_time;
int counter = 0;

void set_clock(const time_t _new_time) {
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);  // Paris timezone - FIXME: make working through DS_TIMEZONE
  tzset();
  new_time = _new_time;
  old_time = new_time;
  struct timeval tv_new_time = {new_time, 0};
  settimeofday(&tv_new_time, NULL);
  delay(100);                                         // Allow new time to propagate  
}

void setup() {
  System::begin();

  // Set system time to some value close to sunset
  set_clock(1000059410);                              // Some date around 2001

  // Illustrate solar events calculation
  auto sunrise = System::getSunrise();
  auto sunset = System::getSunset();
  System::log->printf("Projected sunrise: %02hu:%02hu, sunset: %02hu:%02hu\n",
    sunrise / 60, sunrise % 60, sunset / 60, sunset % 60);

  // Set up two timers: one to turn on the lamp on sunset and another to turn off the lamp 5 mins after sunrise
  TimerSolar timer_sunset(TIMER_SUNSET, "lamp on");
  System::timers.push_front(timer_sunset);
  System::log->println("Timer 1 set at sunset");
  
  TimerSolar timer_sunrise(TIMER_SUNRISE, "lamp off", 5);  
  System::timers.push_front(timer_sunrise);
  System::log->println("Timer 2 set at sunrise + 5 mins");
}


void loop() {

  // Just display current time periodically to see when the timer fires
  new_time = System::getTime();
  if (new_time != old_time) {
    old_time = new_time;
    System::log->println(System::getTimeStr());

    if (counter++ == 15)              // After 15 sec, adjust clock to sunrise
      set_clock(1000099430);          
  }

  System::update();
}