// Wi-Fi Manager example
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

void setup() {
  System::begin();

  // Force network configuration. This can be equally called on an event, e.g., on button press
  System::requestNetworkConfiguration();
}

void loop() {
  if (System::needsNetworkConfiguration()) {
    System::log->println("Entering Wi-Fi Manager");
    System::log->printf("Connect to configuration network '%s', password '%s', and follow instructions\n",
      System::hostname, System::getNetworkConfigPassword().c_str());
    System::configureNetwork(); // This will block until configuration is done
    System::log->println("Exiting Wi-Fi Manager");
  }
  System::update();
}
