// Wi-Fi Manager example, with activation by button
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

void setup() {
  System::begin();
  System::log->println("Long press FLASH button to enter Wi-Fi Manager");
  System::log->printf("Once active, connect to configuration network '%s', password '%s', and follow instructions\n",
    System::hostname, System::getNetworkConfigPassword().c_str());
}

void loop() {
  System::update();
}
