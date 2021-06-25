#ifndef _DS_SYSTEM_H_

#define DS_CAP_SYS_LOG_HW        // Enable syslog on hardware serial line
#define DS_CAP_SYS_TIME          // Enable system time (needed for absolute timers)
#define DS_CAP_SYS_NETWORK       // Enable network (needed for web server)
#define DS_CAP_TIMERS_ABS        // Enable timers from absolute time
#define DS_CAP_TIMERS_SOLAR      // Enable timers from solar events
#define DS_CAP_TIMERS_COUNT_ABS  // Enable countdown timers via absolute time
#define DS_CAP_WEBSERVER         // Enable web server
#define DS_CAP_WEB_TIMERS        // Enable timer configuration via web
#define DS_CAP_SYS_FS            // Enable file system (where timer configuration is stored)

#define DS_TIMEZONE TZ_Europe_Paris  // Timezone. Pick yours from TZ.h coming with ESP Core
#define DS_LONGITUDE 48.85863        // My longitude (needed for solar timers)
#define DS_LATITUDE  2.29443         // My latitude (needed for solar timers)

#include "System.h"         // System global definitions

#endif // _DS_SYSTEM_H_
