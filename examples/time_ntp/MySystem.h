#ifndef _DS_SYSTEM_H_

#define DS_CAP_SYS_NETWORK  // Enable network
#define DS_CAP_SYS_TIME     // Enable system time
#define DS_CAP_SYS_LOG_HW   // Enable syslog on hardware serial line

#define DS_TIMEZONE TZ_Europe_Paris  // Timezone. Pick yours from TZ.h coming with ESP Core

// Optionally, uncomment and define a time server, if different from default (pool.ntp.org)
//#define DS_TIME_SERVER "pool.ntp.org"  // Time server

#include "System.h"         // System global definitions

#endif // _DS_SYSTEM_H_
