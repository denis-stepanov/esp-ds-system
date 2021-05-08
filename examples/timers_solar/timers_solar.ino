// Solar timer example
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

#include <sys/time.h>    // This header is needed to simulate a time sync. Not needed for time actions
#include <TZ.h>          // Same, this is just for a sake of example. Should no longer be required after fix https://github.com/denis-stepanov/esp-ds-system/issues/23

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
int counter = 0;

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

  // Set system time to some value close to sunset
  set_clock(1000059410);                              // Some date around 2001

  // Illustrate solar events calculation
  auto sunrise = System::getSunrise();
  auto sunset = System::getSunset();
  System::log->printf("Projected sunrise: %02hu:%02hu, sunset: %02hu:%02hu\n",
    sunrise / 60, sunrise % 60, sunset / 60, sunset % 60);

  // Set up two timers: one to turn on the lamp on sunset and another to turn off the lamp 5 mins after sunrise
  System::timers.push_front(new TimerSolar("lamp on", TIMER_SUNSET));
  System::log->println("Timer 1 set at sunset");
  
  System::timers.push_front(new TimerSolar("lamp off", TIMER_SUNRISE, 5));
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
