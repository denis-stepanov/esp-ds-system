/* DS System definition
 * * Use capability macros to enable/disable available system features
 * (c) DNS 2020
 */

#ifndef _DS_SYSTEM_H_
#define _DS_SYSTEM_H_

// System capabilities. Define them in MySystem.h before including this header
// DS_CAP_APP_ID       - enable application identification
// DS_CAP_APP_LOG      - enable application log
// DS_CAP_SYS_LED      - enable builtin LED
// DS_CAP_SYS_LOG      - enable syslog
// DS_CAP_SYS_LOG_HW   - enable syslog on hardware serial line (UART 0 or 1)
// DS_CAP_SYS_RESET    - enable software reset interface
// DS_CAP_SYS_RTCMEM   - enable RTC memory
// DS_CAP_SYS_TIME     - enable system time
// DS_CAP_SYS_UPTIME   - enable system uptime counter
// DS_CAP_SYS_FS       - enable file system
// DS_CAP_SYS_NETWORK  - enable networking
// DS_CAP_WIFIMANAGER  - enable Wi-Fi configuration at runtime
// DS_CAP_MDNS         - enable mDNS
// DS_CAP_WEBSERVER    - enable web server
// DS_CAP_BUTTON       - enable button
// DS_CAP_TIMERS_ABS   - enable timers from absolute time
// DS_CAP_TIMERS_SOLAR - enable timers from solar events
// DS_CAP_TIMERS_COUNT - enable countdown timers
// DS_CAP_WEB_TIMERS   - enable timers configuration web form

#include <Arduino.h>          // String

// Consistency checks
#if defined(DS_CAP_SYS_LOG_HW) && !defined(DS_CAP_SYS_LOG)
#define DS_CAP_SYS_LOG
#endif // DS_CAP_SYS_LOG_HW && !DS_CAP_SYS_LOG

#if defined(DS_CAP_APP_LOG) && !defined(DS_CAP_SYS_FS)
#warning "Capability DS_CAP_APP_LOG requires DS_CAP_SYS_FS; enabling"
#define DS_CAP_SYS_FS
#endif // DS_CAP_APP_LOG && !DS_CAP_SYS_FS

#if defined(DS_CAP_WIFIMANAGER) && !defined(DS_CAP_SYS_NETWORK)
#warning "Capability DS_CAP_WIFIMANAGER requires DS_CAP_SYS_NETWORK; enabling"
#define DS_CAP_SYS_NETWORK
#endif // DS_CAP_WIFIMANAGER && !DS_CAP_SYS_NETWORK

#if defined(DS_CAP_MDNS) && !defined(DS_CAP_SYS_NETWORK)
#warning "Capability DS_CAP_MDNS requires DS_CAP_SYS_NETWORK; enabling"
#define DS_CAP_SYS_NETWORK
#endif // DS_CAP_MDNS && !DS_CAP_SYS_NETWORK

#if defined(DS_CAP_WEBSERVER) && !defined(DS_CAP_SYS_NETWORK)
#warning "Capability DS_CAP_WEBSERVER requires DS_CAP_SYS_NETWORK; enabling"
#define DS_CAP_SYS_NETWORK
#endif // DS_CAP_WEBSERVER && !DS_CAP_SYS_NETWORK

#if (defined(DS_CAP_TIMERS_SOLAR) || defined(DS_CAP_TIMERS_COUNT)) && !defined(DS_CAP_TIMERS_ABS)
#define DS_CAP_TIMERS_ABS
#endif // (DS_CAP_TIMERS_SOLAR || DS_CAP_TIMERS_COUNT) && !DS_CAP_TIMERS_ABS

#if defined(DS_CAP_WEB_TIMERS) && !defined(DS_CAP_TIMERS_ABS)
#warning "Capability DS_CAP_WEB_TIMERS requires at least DS_CAP_TIMERS_ABS; enabling"
#define DS_CAP_TIMERS_ABS
#endif // DS_CAP_WEB_TIMERS && !DS_CAP_TIMERS_ABS

#if defined(DS_CAP_TIMERS_ABS) && !defined(DS_CAP_SYS_TIME)
#warning "Selected timer functionality requires time; enabling"
#define DS_CAP_SYS_TIME
#endif // DS_CAP_TIMERS && !DS_CAP_SYS_TIME

// External libraries
#ifdef DS_CAP_SYS_LED
#include <jled.h>                   // LED, https://github.com/jandelgado/jled
#endif // DS_CAP_SYS_LED

#ifdef DS_CAP_SYS_LOG
// Log message prefixed with time (to be used with log.printf())
#define TIMED(MSG) "%010lu: " MSG, millis()
#endif // DS_CAP_SYS_LOG

#ifdef DS_CAP_SYS_RESET
#include <user_interface.h>         // ESP interface
#endif // DS_CAP_SYS_RESET

#ifdef DS_CAP_SYS_FS
#include <FS.h>                     // File system
#endif // DS_CAP_SYS_FS

#ifdef DS_CAP_WEBSERVER
#include <ESP8266WebServer.h>       // Web server
#endif // DS_CAP_WEBSERVER

#ifdef DS_CAP_BUTTON
#include <AceButton.h>              // Button, https://github.com/bxparks/AceButton
#endif // DS_CAP_BUTTON

#ifdef DS_CAP_TIMERS_ABS
#include <forward_list>             // Action list
#endif // DS_CAP_TIMERS_ABS

namespace ds {

#ifdef DS_CAP_SYS_TIME
  typedef enum {
    TIME_SYNC_NONE,                                   // Time was never synchronized
    TIME_SYNC_OK,                                     // Time is synchronized
    TIME_SYNC_DEGRADED                                // Time was synchronized but not anymore
  } time_sync_t;
#endif // DS_CAP_SYS_TIME

#if defined(DS_CAP_TIMERS_ABS) || defined(DS_CAP_TIMERS_COUNT)
  typedef enum {
    TIMER_ABSOLUTE,                                   // Timer fires at a given absolute time
    TIMER_SUNRISE,                                    // Timer fires at sunrise
    TIMER_SUNSET,                                     // Timer fires at sunset
    TIMER_COUNTDOWN,                                  // Timer fires at some moment from now
    TIMER_INVALID                                     // Unsupported timer type (must be the last)
  } timer_type_t;

  class Timer {                                       // Generic timer (abstract)

    protected:
      int id;                                         // Timer identifier (optional)
      timer_type_t type;                              // Timer type
      String label;                                   // Timer label (short description of what it is supposed to do)
      bool armed;                                     // True if timer is armed (will fire); false if ignored with no action
      bool recurrent;                                 // True if timer should be auto-rearmed after firing; false otherwise
      bool transient;                                 // True if timer should be disposed of after firing

      void setType(const timer_type_t /* type */);    // Set timer type

    public:
      Timer(const timer_type_t /* type */, const String label = "undefined",
        const bool armed = true, const bool recurrent = true, const bool transient = false, const int id = -1);  // Constructor
      virtual ~Timer() = 0;                           // Disallow creation of objects of this type
      int getID() const;                              // Return timer identifier
      void setID(const int /* new_id */);             // Set timer identifier
      timer_type_t getType() const;                   // Get timer type
      const String& getLabel() const;                 // Return timer label
      void setLabel(const String& /* new_label */);   // Set timer label
      bool isArmed() const;                           // Return true if timer is armed
      void arm();                                     // Arm the timer (default)
      void disarm();                                  // Disarm the timer
      bool isRecurrent() const;                       // Return true if timer is recurrent
      void repeatForever();                           // Make timer repetitive (default)
      void repeatOnce();                              // Make timer a one-time shot
      bool isTransient() const;                       // Return true if timer is transient (i.e., will be dead after firing)
      void keep();                                    // Keep the timer around (default)
      void forget();                                  // Mark the timer for disposal
      bool operator==(const Timer& /* timer */) const; // Comparison operator
  };
#endif // DS_CAP_TIMERS_ABS || DS_CAP_TIMERS_COUNT

#ifdef DS_CAP_TIMERS_ABS
  typedef enum {
    TIMER_DOW_ANY = -1,                               // Special "day of week" indicating any (or every) day (must be the first)
    TIMER_DOW_SUNDAY,                                 // Sunday
    TIMER_DOW_MONDAY,                                 // Monday
    TIMER_DOW_TUESDAY,                                // Tuesday
    TIMER_DOW_WEDNESDAY,                              // Wednesday
    TIMER_DOW_THURSDAY,                               // Thursday
    TIMER_DOW_FRIDAY,                                 // Friday
    TIMER_DOW_SATURDAY,                               // Saturday
    TIMER_DOW_INVALID                                 // Invalid day of week (must be the last)
  } timer_dow_t;

  class TimerAbsolute : public Timer {                // Absolute time timer (abstract)

    protected:
      struct tm time;                                 // Timer firing details

    public:
      TimerAbsolute(const String label = "undefined", const uint8_t hour = 0, const uint8_t minute = 0, const uint8_t second = 0,
        const timer_dow_t dow = TIMER_DOW_ANY, const bool armed = true, const bool recurrent = true, const bool transient = false,
        const int id = -1);  // Constructor
      virtual ~TimerAbsolute() {}                     // Destructor
      uint8_t getHour() const;                        // Return hour setting
      void setHour(const uint8_t /* new_hour */);     // Set hour setting
      uint8_t getMinute() const;                      // Return minute setting
      void setMinute(const uint8_t /* new_minute */); // Set minute setting
      uint8_t getSecond() const;                      // Return second setting
      void setSecond(const uint8_t /* new_second */); // Set second setting
      int8_t getDayOfWeek() const;                    // Get day of week setting
      void setDayOfWeek(const int8_t /* new_dow */);  // Set day of week setting
      bool operator==(const TimerAbsolute& /* timer */) const; // Comparison operator
      bool operator==(const struct tm& /* _tm */) const; // Time comparison operator
  };
#endif // DS_CAP_TIMERS_ABS

#ifdef DS_CAP_TIMERS_SOLAR
  class TimerSolar : public TimerAbsolute {           // Solar event-based timer

    public:
      TimerSolar(const timer_type_t /* type */, const String label = "undefined", const int8_t offset = 0,
        const uint8_t hour = 0, const uint8_t minute = 0, const uint8_t second = 0, const timer_dow_t dow = TIMER_DOW_ANY,
        const bool armed = true, const bool recurrent = true, const bool transient = false, const int id = -1);  // Constructor
      int8_t getOffset() const;                       // Return offset in minutes from event
      void setOffset(const int8_t /* offset */);      // Set offset in minutes from event
      bool operator==(const TimerSolar& /* timer */) const; // Comparison operator
  };
#endif // DS_CAP_TIMERS_SOLAR

#ifdef DS_CAP_TIMERS_COUNT
  class TimerCountdown : public TimerAbsolute {

    protected:
      // Important: order of fields is important; if bytes come before words, the class will misbehave (g++ bug?)
      time_t next_time;                               // Next time the timer will be fired
      uint8_t ref_hour;                               // Reference hour
      uint8_t ref_minute;                             // Reference minute
      uint8_t ref_second;                             // Reference second

    public:
      TimerCountdown(const String label = "undefined", const uint32_t interval = 1,
        const uint8_t hour = 0, const uint8_t minute = 0, const uint8_t second = 0,
        const bool armed = true, const bool recurrent = true, const bool transient = false, const int id = -1);  // Constructor
      uint8_t getRefHour() const;                     // Return reference hour setting
      void setRefHour(const uint8_t /* new_hour */);  // Set reference hour setting. FIXME: some action needed when ref time gets updated?
      uint8_t getRefMinute() const;                   // Return reference minute setting
      void setRefMinute(const uint8_t /* new_minute */); // Set reference minute setting
      uint8_t getRefSecond() const;                      // Return reference second setting
      void setRefSecond(const uint8_t /* new_second */); // Set reference second setting
      uint32_t getInterval() const;                   // Return timer interval
      void setInterval(const uint32_t /* interval */);// Set timer interval
      void update(const time_t from_time = 0);        // Prepare timer for firing. 0 means from current time
      bool operator==(const TimerCountdown& /* timer */) const; // Comparison operator
  };
#endif // DS_CAP_TIMERS_COUNT

  // Class is just a collection of system-wide routines, so all of them are made static on purpose
  class System {

    public:

      // Shared methods that are always defined
      static void begin();                            // Initialize system
      static void update();                           // Update system
      static String getCapabilities();                // Return list of configured capabilities
      static uint32_t getVersion();                   // Get system version

      // Methods and members specific to capabilities
#ifdef DS_CAP_APP_ID
      static const char *app_name;                    // Application name
      static const char *app_version;                 // Application version
      static const char *app_build;                   // Application build identifier
      static const char *app_url;                     // Application URL
#endif // DS_CAP_APP_ID

#ifdef DS_CAP_APP_LOG
    protected:
      static size_t app_log_size;                     // Application log current size

    public:
      static File app_log;                            // Application log current file
      static size_t app_log_size_max;                 // Maximum size of application log. Setting this to 0 disables log at runtime

      static bool appLogWriteLn(const String& /* line */, bool copy_to_syslog = false); // Write a line into application log, optionally copying to syslog
#endif // DS_CAP_APP_LOG

#ifdef DS_CAP_SYS_LED
      static JLed led;                                // Builtin LED
#endif // DS_CAP_SYS_LED

#ifdef DS_CAP_SYS_LOG
      static Print *log;                              // System log
#endif // DS_CAP_SYS_LOG

#ifdef DS_CAP_SYS_RESET
      static uint32 getResetReason();                 // Return reset reason
      // TODO: add reason string and the reset call itself
#endif // DS_CAP_SYS_RESET

#ifdef DS_CAP_SYS_RTCMEM
      static bool getRTCMem(uint32_t* /* result */, const uint8_t idx = 0, const uint8_t num = 1); // Read 'num' 4 bytes slots from RTC memory offset 'idx' into 'result'
      static bool setRTCMem(const uint32_t* /* source */, const uint8_t idx = 0, const uint8_t num = 1); // Store 'num' 4 bytes slots into RTC memory offset 'idx' from 'source'
#endif // DS_CAP_SYS_RTCMEM

#ifdef DS_CAP_SYS_TIME
    protected:
      static time_t time_sync_time;                   // Last time the time was synchronized
      static void timeSyncHandler();                  // Time sync event handler

    public:
      static time_t getTimeSyncTime();                // Return last time sync time
      static time_sync_t getTimeSyncStatus();         // Return time sync status
      static time_t getTime();                        // Return current time
      static String getTimeStr();                     // Return current time string
      static String getTimeStr(const time_t /* t */); // Return time string for a given time
      static void (*onTimeSync)();                    // Hook to be called when time gets synchronized
#endif // DS_CAP_SYS_TIME

#ifdef DS_CAP_SYS_UPTIME
      static String getUptimeStr();                   // Return uptime as string
#ifdef DS_CAP_SYS_TIME
      static String getBootTimeStr();                 // Return boot time string
#endif // DS_CAP_SYS_TIME
#endif // DS_CAP_SYS_UPTIME

#ifdef DS_CAP_SYS_FS
      static fs::FS& fs;                              // File system control object
#endif // DS_CAP_SYS_FS

#ifdef DS_CAP_SYS_NETWORK
    public:
      static const char *hostname;                    // Hostname; also the SSID of the temporary configuration network
#ifndef DS_CAP_WIFIMANAGER
      static const char *wifi_ssid;                   // Wi-Fi SSID
      static const char *wifi_pass;                   // Wi-Fi password
#endif // !DS_CAP_WIFIMANAGER

      static void connectNetwork(                     // Connect to a known network. LED can be used to signal connection progress
#ifdef DS_CAP_SYS_LED
        JLed *led = nullptr
#endif // DS_CAP_SYS_LED
      );
      static String getNetworkName();                 // Return network name
      static String getNetworkDetails();              // Return network details
      static String getLocalIPAddress();              // Return assigned IP address
#ifdef DS_CAP_SYS_TIME
      static const char *time_server;                 // NTP server (defaults to pool.ntp.org)
      static String getTimeServer();                  // Return NTP server name (possibly, overridden via DHCP)
#endif // DS_CAP_SYS_TIME
      static bool networkIsConnected();               // Return true if network connected
#endif // DS_CAP_SYS_NETWORK

#ifdef DS_CAP_WIFIMANAGER
    protected:
      static bool need_network_config;                // True if network configuration is required

    public:
      static void configureNetwork();                 // Configure network (blocking)
      static bool needsNetworkConfiguration();        // Return true if network needs configuration
      static void requestNetworkConfiguration();      // Request network configuration
      static String getNetworkConfigPassword();       // Return Wi-Fi configuration password
#endif // DS_CAP_WIFIMANAGER

#ifdef DS_CAP_WEBSERVER
    protected:
      static void serveAbout();                       // Serve the "about" page
#ifdef DS_CAP_APP_LOG
      static void serveAppLog();                      // Serve the "log" page
#endif // DS_CAP_APP_LOG

    public:
      static ESP8266WebServer web_server;             // Web server
      static String web_page;                         // Web page buffer
      static void pushHTMLHeader(const String& title = "", const String& head_user = "", bool redirect = false);  // Add standard header to the web page
      static void pushHTMLFooter();                   // Add standard footer to the web page
      static void (*registerWebPages)();              // Hook for registering user-supplied pages

      static void sendWebPage();                      // Send a web page
#endif // DS_CAP_WEBSERVER

#ifdef DS_CAP_BUTTON
    protected:
      static void buttonEventHandler(ace_button::AceButton* /* button */, uint8_t /* event_type */, uint8_t /* button_state */); // Button handler

    public:
      static ace_button::AceButton button;            // Builtin button on pin BUTTON_BUILTIN (0 by default)
      static void (*onButtonInit)();                  // User code to initialize button
      static void (*onButtonPress)(ace_button::AceButton* /* button */, uint8_t /* event_type */, uint8_t /* button_state */); // Hook to be called when button is operated
#endif // DS_CAP_BUTTON

#ifdef DS_CAP_TIMERS_ABS
    public:
      static bool abs_timers_active;                  // True if absolute or solar timers should be served
      static std::forward_list<TimerAbsolute> timers; // List of timers
      static void (*timerHandler)(const TimerAbsolute& /* timer */); // Timer handler
#endif // DS_CAP_TIMERS_ABS

#ifdef DS_CAP_TIMERS_SOLAR
    protected:
      static uint16_t getSolarEvent(const timer_type_t /* ev_type */); // Return solar event time

    public:
      static uint16_t getSunrise();                  // Return sunrise time (in minutes from midnight)
      static uint16_t getSunset();                   // Return sunset time (in minutes from midnight)
#endif // DS_CAP_TIMERS_SOLAR

  };

} // namespace ds

#endif // _DS_SYSTEM_H_
