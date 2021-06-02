// Time monitoring example
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

void setup() {
  System::begin();
  System::setTime(1000000000);                        // Some date around 2001
  delay(100);                                         // Allow new time to propagate
}

void loop() {
  if(System::newSecond())
    System::log->printf("%s > New second has arrived: %d\n",     System::getTimeStr().c_str(), System::tm_time.tm_sec);
  if(System::newMinute())
    System::log->printf("%s >> New minute has arrived: %d\n",    System::getTimeStr().c_str(), System::tm_time.tm_min);
  if(System::newHour())
    System::log->printf("%s >>> New hour has arrived: %d\n",     System::getTimeStr().c_str(), System::tm_time.tm_hour);
  if(System::newDay())
    System::log->printf("%s >>>> New day has arrived: %d\n",     System::getTimeStr().c_str(), System::tm_time.tm_mday);
  if(System::newWeek())
    System::log->printf("%s >>>>> New week has arrived: %d\n",   System::getTimeStr().c_str(), System::tm_time.tm_yday / 7 + 1);
  if(System::newMonth())
    System::log->printf("%s >>>>>> New month has arrived: %d\n", System::getTimeStr().c_str(), System::tm_time.tm_mon + 1);
  if(System::newSecond())
    System::log->printf("%s >>>>>>> Happy New Year %d!\n",       System::getTimeStr().c_str(), System::tm_time.tm_year + 1900);

  System::update();
}
