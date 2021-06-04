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
    System::log->println(System::getTimeStr());

  if(System::newSecond())
    System::log->printf("New second has arrived: %02d\n", System::tm_time.tm_sec);
  if(System::newMinute())
    System::log->printf("New minute has arrived: %02d\n", System::tm_time.tm_min);
  if(System::newHour())
    System::log->printf("New hour has arrived: %02d\n",   System::tm_time.tm_hour);
  if(System::newDay())
    System::log->printf("New day has arrived: %02d\n",    System::tm_time.tm_mday);
  if(System::newWeek())
    System::log->printf("New week has arrived: %02d\n",   System::tm_time.tm_yday / 7 + 1);
  if(System::newMonth())
    System::log->printf("New month has arrived: %02d\n",  System::tm_time.tm_mon + 1);
  if(System::newYear())
    System::log->printf("Happy New Year %d!\n",           System::tm_time.tm_year + 1900);

  System::update();
}
