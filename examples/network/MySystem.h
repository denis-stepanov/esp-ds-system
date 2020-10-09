#ifndef _DS_SYSTEM_H_

#define DS_CAP_SYS_NETWORK  // Enable network
#define DS_CAP_SYS_LOG_HW   // Enable syslog on hardware serial line

// Disable the below if you do not want glowing LED while Wi-Fi is connecting
#define DS_CAP_SYS_LED      // Enable builtin LED
#ifdef ARDUINO_ESP8266_NODEMCU
#define DS_LED_VS_SERIAL_CHECKED_OK // Defaults on NodeMCU are OK (TX=GPIO1, LED=GPIO2)
#endif // ARDUINO_ESP8266_NODEMCU

#include "System.h"         // System global definitions

#endif // _DS_SYSTEM_H_
