#ifndef _DS_SYSTEM_H_

#define DS_CAP_SYS_LOG_HW   // Enable syslog on hardware serial line
#define DS_CAP_SYS_TIME     // Enable system time
#define DS_CAP_TIMERS_SOLAR // Enable timers from solar events

#define DS_TIMEZONE TZ_Europe_Paris  // Timezone. Pick yours from TZ.h coming with ESP Core
#define DS_LONGITUDE 48.85863        // My longitude
#define DS_LATITUDE 2.29443          // My latitude

#include "System.h"         // System global definitions

#endif // _DS_SYSTEM_H_
