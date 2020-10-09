// Web server example
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

// Set network parameters
const char *System::wifi_ssid = "mySSID";             // Network SSID
const char *System::wifi_pass = "myPassword";         // Network password

// Home page
void serveRoot() {
  System::pushHTMLHeader("My Home Page");
  System::web_page += "<p>Hello, world!</p>";
  System::web_page += "<p>Be sure to check <a href=\"/about\">System info</a>.</p>";
  System::pushHTMLFooter();
  System::sendWebPage();
}

// Let web server know we have a page to serve
void registerPages() {
  System::web_server.on("/", serveRoot);
  // .. more pages
}
void (*System::registerWebPages)() = registerPages;

void setup() {
  System::begin();
  if (System::networkIsConnected())
    System::log->printf("Open http://%s in web browser\n", System::getLocalIPAddress().c_str());
  else
    System::log->println("Could not connect to network");
}

void loop() {
  System::update();
}
