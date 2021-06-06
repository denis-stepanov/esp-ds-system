// Solar timer example
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

  // Set system time to some value close to sunset
  System::setTime(1000059410);                        // Some date around 2001
  delay(100);                                         // Allow new time to propagate

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

int counter = 0;

void loop() {

  // Just display current time periodically to see when the timer fires
  if (System::newSecond()) {
    System::log->println(System::getTimeStr());

    if (counter++ == 15) {             // After 15 sec, adjust clock to sunrise
      System::setTime(1000099430);     // Some date around 2001
      delay(100);                      // Allow new time to propagate
    }
  }

  System::update();
}
