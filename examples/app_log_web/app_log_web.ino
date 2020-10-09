// Application log example (with web access)
// Board: NodeMCU 1.0(ESP-12E Module)
// Be sure to enable file system in Arduino settings (Tools -> Flash Size)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

// Set network parameters
const char *System::wifi_ssid = "mySSID";             // Network SSID
const char *System::wifi_pass = "myPassword";         // Network password

void setup() {
  System::begin();
  System::log->printf("Application log configured for max size %zu bytes\n", System::app_log_size_max * 2);
  // x2 because logs get rotated, and we keep the last rotated copy

  // Wait a few sec to get the time sync, so that timestamps in the log make sense
  delay(3000);
  
  String msg = "Log message number 1";
  System::log->printf("Writing app log message: ");
  System::log->println(msg);
  System::appLogWriteLn(msg);
  
  msg = "Log message number 2";
  System::log->println("Example of app log message echoed to syslog:");
  System::appLogWriteLn(msg, true);
  
  System::log->printf("Open http://%s/log in web browser\n", System::getLocalIPAddress().c_str());
}

void loop() {
  System::update();
}
