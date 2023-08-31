#ifndef _DS_SYSTEM_H_

#define DS_CAP_SYS_NETWORK  // Enable network
#define DS_CAP_WEBSERVER    // Enable web server
#define DS_CAP_SYS_LOG_HW   // Enable syslog on hardware serial line

// Uncomment this if you want to customize the web page buffer size (default is 2 kiB pre-allocated in RAM)
// Set this to 0 to avoid pre-allocation altogether and release memory after the page has been served
//#define DS_MAX_WEB_PAGE_SIZE 2048U  // Web page buffer size

#include "System.h"         // System global definitions

#endif // _DS_SYSTEM_H_
