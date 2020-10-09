// Software reset support example
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

void setup() {
  System::begin();
  System::log->print("Reset reason: ");
  switch(System::getResetReason()) {
    case REASON_DEFAULT_RST:      System::log->println("normal startup by power on"); break;
    case REASON_WDT_RST:          System::log->println("hardware watch dog reset"  ); break;
    case REASON_EXCEPTION_RST:    System::log->println("exception reset"           ); break;
    case REASON_SOFT_WDT_RST:     System::log->println("software watch dog reset"  ); break;
    case REASON_SOFT_RESTART:     System::log->println("software restart"          ); break;
    case REASON_DEEP_SLEEP_AWAKE: System::log->println("wake up from deep-sleep"   ); break;
    case REASON_EXT_SYS_RST:      System::log->println("external system reset"     ); break;
    default:                      System::log->println("unknown"                   );
  }
}

void loop() {
}
