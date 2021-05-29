#ifndef _DS_SYSTEM_H_

#define DS_CAP_APP_LOG      // Enable application log
#define DS_CAP_SYS_FS       // Enable file system (where log is stored)
#define DS_CAP_SYS_NETWORK  // Enable network (needed for web server and time sync)
#define DS_CAP_SYS_TIME     // Enable system time (to have timestamps in the log)
#define DS_CAP_WEBSERVER    // Enable web server (to have a log browser)
#define DS_CAP_SYS_LOG_HW   // Enable syslog on hardware serial line

#define DS_TIMEZONE TZ_Europe_Paris  // Timezone. Pick yours from TZ.h coming with ESP Core

#include "System.h"         // System global definitions

#endif // _DS_SYSTEM_H_
