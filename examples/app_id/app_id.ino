// Application identification example
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

/// Application identification
const char *System::app_name    PROGMEM = "Pi Calculator";
const char *System::app_version PROGMEM = "3.14.159";
// Do not initialize "app_build" to demonstrate a default
const char *System::app_url     PROGMEM = "http://www.pi314.net/";

void setup() {
  System::begin();
  System::log->print("Application name   : ");
  System::log->println(System::app_name);
  System::log->print("Application version: ");
  System::log->println(System::app_version);
  System::log->print("Application build  : ");
  System::log->println(System::app_build);
  System::log->print("Application URL    : ");
  System::log->println(System::app_url);
}

void loop() {
}
