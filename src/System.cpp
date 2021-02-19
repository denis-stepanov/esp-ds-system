/* DS System implementation
 * (c) DNS 2020
 */

#include "MySystem.h"        // Read the defined capabilities

using namespace ds;

// Consistency checks (those that can be deferred from the header)
#if defined(DS_CAP_SYS_LED) && defined(DS_CAP_SYS_LOG_HW) && !defined(DS_LED_VS_SERIAL_CHECKED_OK)
#warning "In ESP8266, capabilities DS_CAP_SYS_LED and DS_CAP_SYS_LOG_HW may conflict on a pin. Define DS_LED_VS_SERIAL_CHECKED_OK to suppress this warning"
#endif // DS_CAP_SYS_LED && DS_CAP_SYS_LOG_HW

#if defined(DS_CAP_SYS_TIME) && defined(DS_CAP_SYS_NETWORK) && !defined(DS_TIMEZONE)
#warning "Timezone will be set to UTC. Define DS_TIMEZONE to suppress this warning"
#define DS_TIMEZONE TZ_Etc_UTC
#endif // DS_CAP_SYS_TIME && !DS_TIMEZONE

#if defined(DS_CAP_TIMERS_SOLAR) && !defined(DS_LATITUDE)
#warning "Latitude will be set to Paris. Define DS_LATITUDE to suppress this warning"
#define DS_LATITUDE 48.85863
#endif

#if defined(DS_CAP_TIMERS_SOLAR) && !defined(DS_LONGITUDE)
#warning "Longitude will be set to Paris. Define DS_LONGITUDE to suppress this warning"
#define DS_LONGITUDE 2.29443
#endif




/*************************************************************************
 * Capability: application identification
 *************************************************************************/
// Note that weak symbol initialization at definition is considered undefined behavior, but it seems to work for all variables except references "&"
#ifdef DS_CAP_APP_ID

const char *System::app_name    PROGMEM __attribute__ ((weak)) = "My Program";
const char *System::app_version PROGMEM __attribute__ ((weak)) = "0.1";
const char *System::app_build   PROGMEM __attribute__ ((weak)) = __DATE__ " " __TIME__;
const char *System::app_url     PROGMEM __attribute__ ((weak)) = nullptr;

#endif // DS_CAP_APP_ID




/*************************************************************************
 * Capability: application log
 *************************************************************************/
#ifdef DS_CAP_APP_LOG

static const char *APP_LOG_FILE_NAME  PROGMEM = "/applog.txt";    // Current log file
static const char *APP_LOG_FILE_NAME2 PROGMEM = "/applog2.txt";   // Rotated log file

// Logs tend to fill up the drive. It is better to always keep some space available,
// plus, current implementation will usually overshoot max log size by a few bytes. So reserve some free space
static const size_t APP_LOG_SLACK = 51200;    // Reserve 50kiB

File System::app_log;
size_t System::app_log_size;

// For large file systems, hard-limit log size. It is not likely that more than 1MiB of logs will be needed
size_t System::app_log_size_max __attribute__ ((weak)) = 1048576;

// Write a line into application log
bool System::appLogWriteLn(const String& line, bool copy_to_syslog) {
  bool ret = false;
  String msg;
  if (app_log_size_max) {
#ifdef DS_CAP_SYS_TIME
    msg += getTimeStr();
    msg += F(": ");
#endif // DS_CAP_SYS_TIME
    msg += line;
    ret = app_log.println(msg);
    app_log.flush();
    app_log_size += msg.length();
  }
  if (copy_to_syslog) {
#ifdef DS_CAP_SYS_LOG
    log->printf(TIMED(""));
    log->println(line);
#endif // DS_CAP_SYS_LOG
  }
  return ret;
}

#endif // DS_CAP_APP_LOG




/*************************************************************************
 * Capability: builtin LED
 *************************************************************************/
#ifdef DS_CAP_SYS_LED

JLed System::led(LED_BUILTIN);

#endif // DS_CAP_SYS_LED




/*************************************************************************
 * Capability: system log
 *************************************************************************/
#ifdef DS_CAP_SYS_LOG

#include <Arduino.h>         // millis()

Print* System::log __attribute__ ((weak)) = &Serial;  // Defaults to UART0

#ifdef DS_CAP_SYS_LOG_HW
#include <HardwareSerial.h>  // HardwareSerial log option

static const unsigned int LOG_SPEED = 115200;  // Program log serial line speed (bod)
#endif // DS_CAP_SYS_LOG_HW

#endif // DS_CAP_SYS_LOG




/*************************************************************************
 * Capability: software reset interface
 *************************************************************************/
#ifdef DS_CAP_SYS_RESET

// Return reset reason
uint32 System::getResetReason() {
  return ESP.getResetInfoPtr()->reason;
}

#endif // DS_CAP_SYS_RESET




/*************************************************************************
 * Capability: RTC memory
 *************************************************************************/
#ifdef DS_CAP_SYS_RTCMEM
// API inspired by NodeMCU Lua 'rtcmem' module
// Read 'num' 4 bytes slots from RTC memory offset 'idx' into 'result'
bool System::getRTCMem(uint32_t* result, const uint8_t idx, const uint8_t num) {
  return ESP.rtcUserMemoryRead(idx * sizeof(uint32_t), result, num * sizeof(uint32_t));
}

// Store 'num' 4 bytes slots into RTC memory offset 'idx' from 'source'
bool System::setRTCMem(const uint32_t* source, const uint8_t idx, const uint8_t num) {
  return ESP.rtcUserMemoryWrite(idx * sizeof(uint32_t), const_cast<uint32_t *>(source), num * sizeof(uint32_t));
}
#endif // DS_CAP_SYS_RTCMEM




/*************************************************************************
 * Capability: system time
 *************************************************************************/
#ifdef DS_CAP_SYS_TIME

#include <coredecls.h>       // settimeofday_cb()
#ifdef DS_CAP_SYS_NETWORK
#include <sntp.h>            // SNTP_UPDATE_DELAY
#endif // DS_CAP_SYS_NETWORK

time_t System::time_sync_time = 0;
void (*System::onTimeSync)() __attribute__ ((weak)) = nullptr;

// Time sync event handler
void System::timeSyncHandler() {
#ifdef DS_CAP_SYS_LOG
  log->printf(TIMED("System clock %s: %s\n"), time_sync_time ? "updated" : "set", getTimeStr().c_str());
#endif // DS_CAP_SYS_LOG
#ifdef DS_CAP_APP_LOG
  if (!time_sync_time) {

    // This is not exactly precise, but usually time sync happens shortly after the start; good enough for the app log
    String lmsg = F("Started ");
#ifdef DS_CAP_APP_ID
    lmsg += app_name;
    lmsg += F(" v");
    lmsg += app_version;
    lmsg += F(", build ");
    lmsg += app_build;
#endif // DS_CAP_APP_ID
    appLogWriteLn(lmsg);
  }
#endif // DS_CAP_APP_LOG
  time(&time_sync_time);

  // Call the user hook
  if (onTimeSync)
    onTimeSync();
}

// Return last time sync time
time_t System::getTimeSyncTime() {
  return time_sync_time;
}

// Return time sync status
time_sync_t System::getTimeSyncStatus() {
  const uint32_t update_period =           // Time sync period (s)
#ifdef DS_CAP_SYS_NETWORK
    SNTP_UPDATE_DELAY / 1000
#else
    3600
#endif // DS_CAP_SYS_NETWORK
  ;
  return time_sync_time ? ((unsigned int)(getTime() - time_sync_time) < 2 * update_period ? TIME_SYNC_OK : TIME_SYNC_DEGRADED) : TIME_SYNC_NONE;
}

// Return current time
time_t System::getTime() {
  return time(nullptr);
}

// Return current time string
String System::getTimeStr() {
  auto t = getTime();
  return t ? getTimeStr(t) : F("----/--/-- --:--:--");
}

// Return time string for a given time
String System::getTimeStr(const time_t t) {
  char time_str[20];
  strftime(time_str, sizeof(time_str), "%Y/%m/%d %H:%M:%S", localtime(&t));
  return time_str;
}

#endif // DS_CAP_SYS_TIME




/*************************************************************************
 * Capability: system uptime
 *************************************************************************/
#ifdef DS_CAP_SYS_UPTIME

#include <uptime.h>                 // https://github.com/YiannisBourkelis/Uptime-Library

// Return uptime as string
//// Note that in order to count uptime correctly this function has to be called at least once every 49 days (see the library source)
//// TODO: add this as a periodic routine
String System::getUptimeStr() {
  String str;
  uptime::calculateUptime();
  const auto days = uptime::getDays(),
             hours = uptime::getHours(),
             minutes = uptime::getMinutes(),
             seconds = uptime::getSeconds();
  if (days) {
    str += days;
    str += F(" day");
    str += days % 10 == 1 && days != 11 ? F("") : F("s");
    str += F(" ");
  }
  if (hours < 10)
    str += F("0");
  str += hours;
  str += F(":");
  if (minutes < 10)
    str += F("0");
  str += minutes;
  str += F(":");
  if (seconds < 10)
    str += F("0");
  str += seconds;
  return str;
}

#ifdef DS_CAP_SYS_TIME
// Return boot time string
String System::getBootTimeStr() {
  uptime::calculateUptime();
  auto t = getTime();
  return t ? getTimeStr(t - (((uptime::getDays() * 24 + uptime::getHours()) * 60 + uptime::getMinutes()) * 60 + uptime::getSeconds())) : F("----/--/-- --:--:--");
}
#endif // DS_CAP_SYS_TIME

#endif // DS_CAP_SYS_UPTIME




/*************************************************************************
 * Capability: file system
 *************************************************************************/
#ifdef DS_CAP_SYS_FS

#include <LittleFS.h>                      // LittleFS support

fs::FS& System::fs = LittleFS;             // Use LittleFS as file system

#endif // DS_CAP_SYS_FS




/*************************************************************************
 * Capability: networking
 *************************************************************************/
#ifdef DS_CAP_SYS_NETWORK

#include <ESP8266WiFi.h>     // WiFi object

const char *System::hostname PROGMEM __attribute__ ((weak)) = "espDS";
static const unsigned long NETWORK_CONNECT_TIMEOUT = 20000; // (ms)

#ifdef DS_CAP_SYS_TIME
#ifdef DS_CAP_WEBSERVER
static const char *DS_TIMEZONE_STRING PROGMEM = __XSTRING(DS_TIMEZONE);  // Important that this is initialized before TZ.h is included
#endif // DS_CAP_WEBSERVER
#include <TZ.h>              // Timezones
#include <sntp.h>            // SNTP server

const char *System::time_server PROGMEM __attribute__ ((weak)) = "pool.ntp.org";
#endif // DS_CAP_SYS_TIME

// Connect to a known network. LED can be used to signal connection progress
void System::connectNetwork(
#ifdef DS_CAP_SYS_LED
  JLed *led
#endif // DS_CAP_SYS_LED
  ) {

#ifdef DS_CAP_WIFIMANAGER
  if(getNetworkName()) {
#endif // DS_CAP_WIFIMANAGER

#ifdef DS_CAP_SYS_LOG
    log->printf(TIMED("Connecting to network '"));
    log->print(
#ifdef DS_CAP_WIFIMANAGER
      getNetworkName()
#else
      wifi_ssid
#endif // DS_CAP_WIFIMANAGER
      );
    log->printf("'... ");
#endif // DS_CAP_SYS_LOG

    WiFi.mode(WIFI_STA);
    WiFi.hostname(hostname);
    WiFi.begin(
#ifndef DS_CAP_WIFIMANAGER
      wifi_ssid, wifi_pass
#endif
    );

#ifdef DS_CAP_SYS_LED
    if (led)
      led->Breathe(1000).Forever();      // Signal connection process with glowing
#endif // DS_CAP_SYS_LED

    auto t0 = millis();
    while (!networkIsConnected() && millis() - t0 < NETWORK_CONNECT_TIMEOUT) {
#ifdef DS_CAP_SYS_LED
      if (led)
        led->Update();
#endif // DS_CAP_SYS_LED
      yield();
    }
#ifdef DS_CAP_SYS_LED
    if (led)
      led->Off().Update();
    if (!networkIsConnected() && led) {
      led->Blink(100, 100).Repeat(3);    // Signal problem with three blinks
      while(led->Update())
        yield();
    }
#endif // DS_CAP_SYS_LED

#ifdef DS_CAP_SYS_LOG
    if (networkIsConnected()) {
      log->print(F("connected. IP address: "));
      log->println(getLocalIPAddress());
    }
    else
      log->println(F("connection timeout"));
#endif // DS_CAP_SYS_LOG

#ifdef DS_CAP_WIFIMANAGER
  }
#ifdef DS_CAP_SYS_LOG
  else
    log->printf(TIMED("Skipping network connection, as the network is not configured. Use Wi-Fi Manager to configure\n"));
#endif // DS_CAP_SYS_LOG
#endif // DS_CAP_WIFIMANAGER

#ifdef DS_CAP_SYS_TIME
  // Kick off NTP synchronization
#ifdef DS_CAP_SYS_LOG
  log->printf(TIMED("Starting NTP client service... "));
#endif // DS_CAP_SYS_LOG
  configTime(DS_TIMEZONE, time_server);
#ifdef DS_CAP_SYS_LOG
  log->println(F("OK"));
#endif // DS_CAP_SYS_LOG
#endif // DS_CAP_SYS_TIME
}

// Return network name
String System::getNetworkName() {
  return WiFi.SSID();
}

// Return network details
String System::getNetworkDetails() {
  String details = F("Wi-Fi channel: ");
  details += WiFi.channel();
  details += F(", RSSI: ");
  details += WiFi.RSSI();
  details += F(" dBm");
  return details;
}

// Return assigned IP address
String System::getLocalIPAddress() {
  return WiFi.localIP().toString();
}

#ifdef DS_CAP_SYS_TIME
// Return NTP server name (possibly, overridden via DHCP)
String System::getTimeServer() {
  return sntp_getservername(0);
}
#endif // DS_CAP_SYS_TIME

// Return true if connected
bool System::networkIsConnected() {
  return WiFi.isConnected();
}

#endif // DS_CAP_SYS_NETWORK




/*************************************************************************
 * Capability: Wi-Fi configuration at runtime
 *************************************************************************/
#ifdef DS_CAP_WIFIMANAGER

#include <WiFiManager.h>     // https://github.com/tzapu/WiFiManager

bool System::need_network_config = false;

// Configure network (blocking)
void System::configureNetwork() {

#ifdef DS_CAP_SYS_LOG
  log->printf(TIMED("Entering network configuration\n"));
#endif // DS_CAP_SYS_LOG

#ifdef DS_CAP_SYS_LED
  led.On().Update();
#endif // DS_CAP_SYS_LED

  WiFiManager wifiManager;
  wifiManager.startConfigPortal(hostname, getNetworkConfigPassword().c_str());
  need_network_config = false;

#ifdef DS_CAP_SYS_LED
  led.Off().Update();
#endif // DS_CAP_SYS_LED

#ifdef DS_CAP_SYS_LOG
  log->printf(TIMED("Network reconfigured to \"%s\"\n"), getNetworkName().c_str());
#endif // DS_CAP_SYS_LOG

#ifdef DS_CAP_APP_LOG
  String lmsg = F("Network reconfigured to \"");
  lmsg += getNetworkName();
  lmsg += F("\"");
  appLogWriteLn(lmsg);
#endif // DS_CAP_APP_LOG
}

// Return true if network needs configuration
bool System::needsNetworkConfiguration() {
  return need_network_config;
}

// Request network configuration
void System::requestNetworkConfiguration() {
  need_network_config = true;
}

// Return Wi-Fi configuration password
String System::getNetworkConfigPassword() {

  // Make 8 chars password based on hostname
  String pass(F("42"));
  pass += hostname;
  while (pass.length() < 8)
    pass += '0';     // Pad the pass if the hostname is too short
  pass.remove(8);
  return pass;
}

#endif // DS_CAP_WIFIMANAGER




/*************************************************************************
 * Capability: mDNS
 *************************************************************************/
#ifdef DS_CAP_MDNS

#include <ESP8266mDNS.h>            // mDNS support

#endif // DS_CAP_MDNS




/*************************************************************************
 * Capability: web server
 *************************************************************************/
#ifdef DS_CAP_WEBSERVER

#include <ESP8266HTTPClient.h>      // HTTP_CODE_*

ESP8266WebServer System::web_server;
String System::web_page((char *)nullptr);        // Avoid initial memory allocation
void (*System::registerWebPages)() __attribute__ ((weak)) = nullptr;

#ifdef DS_CAP_SYS_FS
static const char *FAV_ICON_PATH PROGMEM = "/favicon.png"; // Favicon on disk
#endif // DS_CAP_SYS_FS
static const size_t MAX_WEB_PAGE_SIZE = 2048;    // Preallocated web page buffer size (B)

// Add standard header to the web page
void System::pushHTMLHeader(const String& title, const String& head_user, bool redirect) {
  web_page = F(
    "<!DOCTYPE html>\n"
    "<html><head><title>");
  web_page += title;
  web_page += F(
    "</title>\n"
    "<meta charset=\"UTF-8\"/>\n"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/>\n");
  if (redirect)
    web_page += F("<meta http-equiv=\"Refresh\" content=\"5; /\"/>\n");
#ifdef DS_CAP_SYS_FS
  if (fs.exists(FAV_ICON_PATH)) {
    web_page += F("<link rel=\"icon\" type=\"image/png\" href=\"");
    web_page += FAV_ICON_PATH;
    web_page += F("\" sizes=\"192x192\">\n");
  }
#endif // DS_CAP_SYS_FS
  web_page += head_user;
  web_page += F(
    "</head>\n"
    "<body>");
}

// Add standard footer to the web page
void System::pushHTMLFooter() {
  web_page += F("<hr/></body></html>");
}

// Serve the "about" page
#define TR_BEGIN(LABEL) F("<tr><td>" LABEL "</td><td>")
#define TR_END F("</td></tr>\n")
void System::serveAbout() {
  pushHTMLHeader(F("System Information"));
  web_page += F(
    "<h3>System Information</h3>\n"
    "[ <a href=\"/\">home</a> ]<hr/>\n"
    "<table border=\"1\" cellpadding=\"3\" cellspacing=\"0\" style=\"border-collapse: collapse;\">\n"
  );

#ifdef DS_CAP_APP_ID
  web_page += TR_BEGIN("Program");
  if (app_url) {
    web_page += F("<a href=\"");
    web_page += app_url;
    web_page += F("\">");
  }
  web_page += app_name;
  if (app_url)
    web_page += F("</a>");
  web_page += F(", v");
  web_page += app_version;
  if (app_build) {
    web_page += F(", build ");
    web_page += app_build;
  }
  web_page += TR_END;
#endif // DS_CAP_APP_ID

  web_page += TR_BEGIN("Hardware");
  web_page += F("CPU: ");
  web_page += ESP.getCpuFreqMHz();
  web_page += F(" MHz, flash memory: ");
  web_page += ESP.getFlashChipSize() / 1024;
  web_page += F(" kB, ");
  web_page += ESP.getFlashChipSpeed() / 1000000;
  web_page += F(" MHz ");
  switch (ESP.getFlashChipMode()) {
    case FM_QIO:  web_page += F("QIO");  break;
    case FM_QOUT: web_page += F("QOUT"); break;
    case FM_DIO:  web_page += F("DIO");  break;
    case FM_DOUT: web_page += F("DOUT"); break;
    default: ;
  }
  web_page += TR_END;

  web_page += TR_BEGIN("Memory Heap Status");
  uint32_t free_mem = 0;
  uint16_t max_block = 0;
  uint8_t frag = 0;
  ESP.getHeapStats(&free_mem, &max_block, &frag);
  web_page += free_mem;
  web_page += F(" B free, max block: ");
  web_page += max_block;
  web_page += F(" B, fragmentation ");
  web_page += frag;
  web_page += F("%");
  web_page += TR_END;

#ifdef DS_CAP_SYS_FS
  FSInfo fsi;
  fs.info(fsi);
  web_page += TR_BEGIN("File System");
  if (&fs == &LittleFS)
    web_page += F("LittleFS");
  else if (&fs == &SPIFFS)
    web_page += F("SPIFFS");
  else
    web_page += F("Unknown");
  web_page += F(", ");
  web_page += fsi.totalBytes / 1024;
  web_page += F(" kB (");
  web_page += 100 * fsi.usedBytes / fsi.totalBytes;
  web_page += F("% use)");
  web_page += TR_END;
#endif // DS_CAP_SYS_FS

#ifdef DS_CAP_APP_LOG
  web_page += TR_BEGIN("Application Log");
  if (app_log_size_max) {
    web_page += app_log_size / 1024;
    web_page += F(" / ");
    web_page += app_log_size_max / 1024;
    web_page += F(" kB used");
  } else
    web_page += F("Disabled");
  web_page += TR_END;
#endif // DS_CAP_APP_LOG

  web_page += TR_BEGIN("Firmware");
  String fw = ESP.getFullVersion();
  fw.replace('/', ' ');              // For better page layout
  web_page += fw;
  web_page += TR_END;

  web_page += TR_BEGIN("DS System");
  web_page += F("v");
  web_page += getVersion();
  web_page += F(", capabilities: ");
  web_page += getCapabilities();
  web_page += TR_END;

#ifdef DS_CAP_SYS_NETWORK
  web_page += TR_BEGIN("Connected to Network");
  web_page += getNetworkName();
  web_page += F(", ");
  web_page += getNetworkDetails();
  web_page += TR_END;

  web_page += TR_BEGIN("IP Address");
  web_page += getLocalIPAddress();
  web_page += TR_END;
#endif // DS_CAP_SYS_NETWORK

#ifdef DS_CAP_MDNS
  web_page += TR_BEGIN("mDNS Hostname");
  web_page += hostname;
  web_page += F(".local");
  web_page += TR_END;
#endif // DS_CAP_MDNS

#ifdef DS_CAP_WIFIMANAGER
  web_page += TR_BEGIN("Wi-Fi Config AP");
  web_page += F("SSID: ");
  web_page += hostname;
  web_page += F(", password: ");
  web_page += getNetworkConfigPassword();
  web_page += TR_END;
#endif // DS_CAP_WIFIMANAGER

#ifdef DS_CAP_SYS_TIME
  web_page += TR_BEGIN("System Time");
  web_page += getTimeStr();
  web_page += F(", ");
  web_page += DS_TIMEZONE_STRING;
  web_page += TR_END;

  web_page += TR_BEGIN("Time Sync Status");
  auto sync_status = getTimeSyncStatus();
  switch (sync_status) {
    case TIME_SYNC_NONE:     web_page += F("Not synchronized"); break;
    case TIME_SYNC_OK:       web_page += F("Synchronized");     break;
    case TIME_SYNC_DEGRADED: web_page += F("Degraded");         break;
  }
  web_page += F(". Last sync: ");
  web_page += getTimeStr(getTimeSyncTime());
#ifdef DS_CAP_SYS_NETWORK
  web_page += F(", NTP server: ");
  web_page += getTimeServer();
#endif // DS_CAP_SYS_NETWORK
  web_page += TR_END;
#endif // DS_CAP_SYS_TIME

#ifdef DS_CAP_SYS_UPTIME
  web_page += TR_BEGIN("System Uptime");
  web_page += getUptimeStr();
#ifdef DS_CAP_SYS_TIME
  if (sync_status != TIME_SYNC_NONE) {
    web_page += F(", booted ");
    web_page += getBootTimeStr();
  }
#endif // DS_CAP_SYS_TIME
  web_page += TR_END;
#endif // DS_CAP_SYS_UPTIME

#ifdef DS_CAP_SYS_LOG_HW
  web_page += TR_BEGIN("Serial Log Link");
  web_page += LOG_SPEED;
  web_page += F("/8-N-1");
  web_page += TR_END;
#endif // DS_CAP_SYS_LOG_HW

  web_page += F("</table>\n");
  pushHTMLFooter();
  sendWebPage();
}

#ifdef DS_CAP_APP_LOG
// Serve the "log" page
const size_t APP_LOG_PAGE_SIZE = 1024;     // Max amount of information to display from the log file (B)
void System::serveAppLog() {
  pushHTMLHeader(F("Application Log"));
  web_page += F(
    "<h3>Application Log</h3>\n"
    "[ <a href=\"/\">home</a> ]<hr/>\n"
  );

  if (app_log_size_max) {

    // Parse query params
    unsigned int log_page = 0;
    String log_file_name(APP_LOG_FILE_NAME), log_param;
    for (unsigned int i = 0; i < (unsigned int)web_server.args(); i++) {
      const String argname = web_server.argName(i);
      if (argname == "p")
        log_page = web_server.arg(i).toInt();
      else
        if (argname == "r") {
          log_file_name = APP_LOG_FILE_NAME2;
          log_param = "r=1&";
        }
    }

    File log_file = fs.open(log_file_name, "r");
    if (log_file) {
      const size_t fsize = log_file.size();

      // Print pagination buttons
      if (fsize > (log_page + 1) * APP_LOG_PAGE_SIZE) {
        web_page += F("[ <a href=\"/log?");
        web_page += log_param;
        web_page += F("p=");
        web_page += log_page + 1;
        web_page += F("\">&lt;&lt;</a> ]&nbsp;&nbsp;&nbsp;\n");
      } else {
        if (log_file_name == APP_LOG_FILE_NAME && fs.exists(APP_LOG_FILE_NAME2))
          web_page += F("[ <a href=\"/log?r=1\">&lt;&lt;</a> ]&nbsp;&nbsp;&nbsp;\n");
        else
          web_page += F("[ &lt;&lt; ]&nbsp;&nbsp;&nbsp;\n");
      }
      if (log_page) {
        web_page += F("[ <a href=\"/log?");
        web_page += log_param;
        web_page += F("p=");
        web_page += log_page - 1;
        web_page += F("\">&gt;&gt;</a> ]\n");
      } else {
        if (log_file_name == APP_LOG_FILE_NAME2 && fs.exists(APP_LOG_FILE_NAME)) {
          File log_file_next = fs.open(APP_LOG_FILE_NAME, "r");
          if (log_file_next) {
            const size_t log_page_next = log_file_next.size() / APP_LOG_PAGE_SIZE;
            log_file_next.close();
            web_page += F("[ <a href=\"/log?p=");
            web_page += log_page_next;
            web_page += F("\">&gt;&gt;</a> ]\n");
          } else
            web_page += F("[ &gt;&gt; ]\n");
        } else
          web_page += F("[ &gt;&gt; ]\n");
      }

      // Print log fragment
      web_page += F("<span style=\"font-family: monospace;\">\n");

      if (fsize > (log_page + 1) * APP_LOG_PAGE_SIZE) {
        log_file.seek(fsize - (log_page + 1) * APP_LOG_PAGE_SIZE);
        log_file.readStringUntil('\n');
      }
      String line;
      while (log_file.available() && log_file.position() <= fsize - log_page * APP_LOG_PAGE_SIZE) {
        line = log_file.readStringUntil('\n');
        web_page += F("<br>");
        web_page += line;   // already contains '\n'
      }
      log_file.close();
    } else
      web_page += F("<span><br/>Log opening error");

    web_page += F("</span>\n");
  } else
    web_page += F("<br/>Logging is disabled (missing or full file system)");
  pushHTMLFooter();
  sendWebPage();
}
#endif // DS_CAP_APP_LOG

// Send a web page
void System::sendWebPage() {

#ifdef DS_CAP_SYS_LOG
  log->printf(TIMED("Serving webpage \""));
  log->print(web_server.uri());
  log->print(F("\" to "));
  log->println(web_server.client().remoteIP().toString());
#endif // DS_CAP_SYS_LOG

  web_server.send(HTTP_CODE_OK, "text/html", web_page);
}

#endif // DS_CAP_WEBSERVER




/*************************************************************************
 * Capability: builtin button
 *************************************************************************/
#ifdef DS_CAP_BUTTON

using namespace ace_button;

#ifndef BUTTON_BUILTIN
#define BUTTON_BUILTIN ((uint8_t)0)        // Default button is on pin 0. For the typecast, see https://github.com/bxparks/AceButton/issues/40
#endif // BUTTON_BUILTIN

AceButton System::button(BUTTON_BUILTIN);  // Builtin button
void (*System::onButtonInit)() __attribute__ ((weak)) = nullptr;
void (*System::onButtonPress)(AceButton*, uint8_t, uint8_t) __attribute__ ((weak)) = nullptr;

// Button handler
void System::buttonEventHandler(AceButton* button, uint8_t event_type, uint8_t button_state) {
  switch (event_type) {

#ifdef DS_CAP_WIFIMANAGER
    case AceButton::kEventLongPressed:    // Wi-Fi configuration
      requestNetworkConfiguration();
      break;
#endif // DS_CAP_WIFIMANAGER

  }
  if (onButtonPress)
    onButtonPress(button, event_type, button_state);
}

#endif // DS_CAP_BUTTON




/*************************************************************************
 * Capability: timers from absolute time
 *************************************************************************/
#ifdef DS_CAP_TIMERS_ABS

bool System::abs_timers_active = true;               // Activate timers
std::forward_list<TimerAbsolute> System::timers;
void (*System::timerHandler)(const TimerAbsolute& /* timer */) __attribute__ ((weak)) = nullptr;

// struct tm (re)use:
//   int tm_sec;    - timer firing second (0..59)
//   int tm_min;    - timer firing minute (0..59)
//   int tm_hour;   - timer firing hour (0..23)
//   int tm_mday;   - solar timer: timer offset (-59..+59 min)
//       >>         - countdown timer: timer offset (0..86399 s)
//   int tm_mon;    - countdown timer: time interval (1..86400 s)
//   int tm_year;   - (not used)
//   int tm_wday;   - timer firing day of the week (-1..6, Sunday=0, -1=every day)
//   int tm_yday;   - (not used)
//   int tm_isdst;  - countdown timer: next firing time (time_t)

// Timer constructor
Timer::Timer(const timer_type_t _type, const String _label,
  const bool _armed, const bool _recurrent, const bool _transient, const int _id) :
  id(_id >= -1 ? _id : -1), type(_type >= 0 && _type <= TIMER_INVALID ? _type : TIMER_INVALID),
  label(_label), armed(_armed), recurrent(_recurrent), transient(_transient) {}

// Define pure virtual destructor (required by C++)
Timer::~Timer() {
}

// Return timer identifier
int Timer::getID() const {
  return id;
}

// Set timer identifier
void Timer::setID(const int new_id) {
  if (new_id >= -1)
    id = new_id;
}

// Get timer type
timer_type_t Timer::getType() const {
  return type;
}

// Set timer type
void Timer::setType(const timer_type_t _type) {
  type = _type >= 0 && _type <= TIMER_INVALID ? _type : TIMER_INVALID;
}

// Return timer label
const String& Timer::getLabel() const {
  return label;
}

// Set timer label
void Timer::setLabel(const String& new_label) {
  label = new_label;
}

// Return true if timer is armed
bool Timer::isArmed() const {
  return armed;
}

// Arm the timer (default)
void Timer::arm() {
  armed = true;
}

// Disarm the timer
void Timer::disarm() {
  armed = false;
}

// Return true if timer is recurrent
bool Timer::isRecurrent() const {
  return recurrent;
}

// Make timer repetitive (default)
void Timer::repeatForever() {
  recurrent = true;
}

// Make timer a one-time shot
void Timer::repeatOnce() {
  recurrent = false;
}

// Return true if timer is transient (i.e., will be dead after firing)
bool Timer::isTransient() const {
  return transient;
}

// Keep the timer around (default)
void Timer::keep() {
  transient = false;
}

// Mark the timer for disposal
void Timer::forget() {
  transient = true;
}

// Abstract timer comparison operator
bool Timer::operator==(const Timer& timer) const {
  return type == timer.getType() && id == timer.getID() && label == timer.getLabel();
}

// Absolute timer constructor
TimerAbsolute::TimerAbsolute(const String label, const uint8_t hour, const uint8_t minute, const uint8_t second,
  const timer_dow_t dow, const bool armed, const bool recurrent, const bool transient, const int id) :
  Timer(TIMER_ABSOLUTE, label, armed, recurrent, transient, id),
  time({second <= 59 ? second : 0, minute <= 59 ? minute : 0, hour <= 23 ? hour : 0,
    0, 0, 0, dow >= TIMER_DOW_ANY && dow <= TIMER_DOW_INVALID ? dow : TIMER_DOW_INVALID, 0, 0}) {}

// Return hour setting
uint8_t TimerAbsolute::getHour() const {
  return time.tm_hour;
}

// Set hour setting
void TimerAbsolute::setHour(const uint8_t new_hour) {
  if (new_hour <= 23)
    time.tm_hour = new_hour;
}

// Return minute setting
uint8_t TimerAbsolute::getMinute() const {
  return time.tm_min;
}

// Set minute setting
void TimerAbsolute::setMinute(const uint8_t new_minute) {
  if (new_minute <= 59)
    time.tm_min = new_minute;
}

// Return second setting
uint8_t TimerAbsolute::getSecond() const {
  return time.tm_sec;
}

// Set second setting
void TimerAbsolute::setSecond(const uint8_t new_second) {
  if (new_second <= 59)
    time.tm_sec = new_second;
}

// Get day of week setting
int8_t TimerAbsolute::getDayOfWeek() const {
  return time.tm_wday;
}

// Set day of week setting
void TimerAbsolute::setDayOfWeek(const int8_t new_dow) {
  if (new_dow >= TIMER_DOW_ANY && new_dow <= TIMER_DOW_INVALID)
    time.tm_wday = new_dow;
}

// Absolute timer comparison operator
bool TimerAbsolute::operator==(const TimerAbsolute& timer) const {
  return Timer::operator==(timer) && getHour() == timer.getHour() &&
    getMinute() == timer.getMinute() && getSecond() == timer.getSecond() && getDayOfWeek() == timer.getDayOfWeek();
}

// Time comparison operator
bool TimerAbsolute::operator==(const struct tm& _tm) const {
  return getHour() == _tm.tm_hour && getMinute() == _tm.tm_min && getSecond() == _tm.tm_sec;
}

#endif // DS_CAP_TIMERS_ABS




/*************************************************************************
 * Capability: timers from solar events
 *************************************************************************/
#ifdef DS_CAP_TIMERS_SOLAR

#include <Dusk2Dawn.h>              // Sunrise/sunset calculation, https://github.com/dmkishi/Dusk2Dawn  (! get the latest master via ZIP, not v1.0.1 from Arduino IDE !)

// Solar timer constructor
TimerSolar::TimerSolar(const timer_type_t _type, const String label, const int8_t offset,
  const uint8_t hour, const uint8_t minute, const uint8_t second,
  const timer_dow_t dow, const bool armed, const bool recurrent, const bool transient, const int id) :
  TimerAbsolute(label, hour, minute, second, dow, armed, recurrent, transient, id) {
  setType(_type == TIMER_SUNRISE || _type == TIMER_SUNSET ? _type : TIMER_INVALID);
  setOffset(offset >= -59 && offset <= 59 ? offset : 0);
}

// Return offset in minutes from event
int8_t TimerSolar::getOffset() const {
  return time.tm_mday;
}

// Set offset in minutes from event
void TimerSolar::setOffset(const int8_t offset) {
  if (offset >= -59 && offset <= 59)
    time.tm_mday = offset;
}

// Solar timer comparison operator
bool TimerSolar::operator==(const TimerSolar& timer) const {
  return Timer::operator==(timer) && getDayOfWeek() == timer.getDayOfWeek() && getOffset() == timer.getOffset();
}

uint16_t System::getSolarEvent(const timer_type_t ev_type) {
  struct tm tm_local, tm_gmt;

  const auto cur_time = getTime();
  localtime_r(&cur_time, &tm_local);
  gmtime_r(&cur_time, &tm_gmt);
  Dusk2Dawn local(DS_LATITUDE, DS_LONGITUDE, (mktime(&tm_local) - mktime(&tm_gmt)) / 60.0 / 60);
  return ev_type == TIMER_SUNRISE ?
    local.sunrise(tm_local.tm_year + 1900, tm_local.tm_mon + 1, tm_local.tm_mday, tm_local.tm_isdst) :
    local.sunset (tm_local.tm_year + 1900, tm_local.tm_mon + 1, tm_local.tm_mday, tm_local.tm_isdst);
}

// Return sunrise time (in minutes from midnight)
uint16_t System::getSunrise() {
  return getSolarEvent(TIMER_SUNRISE);
}

// Return sunset time (in minutes from midnight)
uint16_t System::getSunset() {
  return getSolarEvent(TIMER_SUNSET);
}

#endif // DS_CAP_TIMERS_SOLAR




/*************************************************************************
 * Capability: countdown timers
 *************************************************************************/
#ifdef DS_CAP_TIMERS_COUNT

// Countdown timer constructor
TimerCountdown::TimerCountdown(const String label, const uint32_t interval, const uint32_t offset,
  const bool armed, const bool recurrent, const bool transient, const int id) :
  TimerAbsolute(label, 0, 0, 0, TIMER_DOW_ANY, armed, recurrent, transient, id) {
    setType(TIMER_COUNTDOWN);
    setInterval(interval <= 24 * 60 * 60 ? (interval > 0 ? interval : 1) : 24 * 60 * 60);
    setOffset(offset < 24 * 60 * 60 ? offset : 24 * 60 * 60 - 1);
    setNextTime(0);  // Next firing time. Setting it to 0 will force recalculation
    update();
}

// Return next firing time
time_t TimerCountdown::getNextTime() const {
  return time.tm_isdst;
}

// Set next firing time
void TimerCountdown::setNextTime(const time_t new_time) {
  time.tm_isdst = new_time;
}

// Return timer interval
uint32_t TimerCountdown::getInterval() const {
  return time.tm_mon;
}

// Set timer interval
void TimerCountdown::setInterval(const uint32_t interval) {
  if (interval > 0 && interval <= 24 * 60 * 60)
    time.tm_mon = interval;
}

// Return timer offset in seconds from midnight
uint32_t TimerCountdown::getOffset() const {
  return time.tm_mday;
}

// Set timer offset in seconds from midnight
void TimerCountdown::setOffset(const uint32_t offset) {
  if (offset < 24 * 60 * 60)
    time.tm_mday = offset;
}

// Prepare timer for firing
void TimerCountdown::update(const time_t from_time) {
  const auto interval = getInterval();
  auto next_time = getNextTime();
  const auto cur_time = from_time ? from_time : System::getTime();
  if (next_time > cur_time && next_time - cur_time < (int) interval)
    return;     // Countdown goes as planned

  if (next_time == cur_time) {     // Timer fired
    setNextTime(next_time + interval);
    auto next = getSecond() + interval;
    setSecond(next % 60);
    auto leap = next / 60;
    next = getMinute() + leap;
    setMinute(next % 60);
    leap = next / 60;
    setHour((getHour() + leap) % 24);
    return;
  }

  // Otherwise we are out of sync and need to rebase the timer
  const auto offset = getOffset();
  struct tm tm_now, tm_ref;
  localtime_r(&cur_time, &tm_now);
  tm_ref = tm_now;
  tm_ref.tm_hour = offset / (60 * 60);
  tm_ref.tm_min = (offset - 60 * 60 * tm_ref.tm_hour) / 60;
  tm_ref.tm_sec = offset % 60;
  const auto tdiff = interval - abs(cur_time - mktime(&tm_ref)) % interval;
  setNextTime(cur_time + tdiff);
  setSecond((tm_now.tm_sec + tdiff) % 60);
  auto leap = (tm_now.tm_sec + tdiff) / 60;
  setMinute((tm_now.tm_min + leap) % 60);
  leap = (tm_now.tm_min + leap) / 60;
  setHour((tm_now.tm_hour + leap) % 24);
}

// Countdown timer comparison operator
bool TimerCountdown::operator==(const TimerCountdown& timer) const {
  return Timer::operator==(timer) && getInterval() == timer.getInterval();
}

#endif // DS_CAP_TIMERS_COUNT




/*************************************************************************
 * Capability: timers configuration web form
 *************************************************************************/

// FIXME



/*************************************************************************
 * Shared methods that are always defined
 *************************************************************************/
// Initialize system
void System::begin() {

#ifdef DS_CAP_SYS_LOG
#ifdef DS_CAP_SYS_LOG_HW
  // This could be better done with dynamic_cast instead of #define, but default compiler options do not allow RTTI
  static_cast<HardwareSerial *>(log)->begin(LOG_SPEED);
#endif // DS_CAP_SYS_LOG_HW
#ifdef DS_CAP_APP_ID
  log->printf("\n\n" TIMED("Started %s v%s, build %s\n"), app_name, app_version, app_build);
#else
  log->printf("\n\n" TIMED("Started\n"));
#endif // DS_CAP_APP_ID
#endif // DS_CAP_SYS_LOG

// Corresponding application log entry will be written once the time is synchronized

#ifdef DS_CAP_SYS_LED
#ifdef DS_CAP_SYS_LOG
  log->printf(TIMED("Initializing builtin LED... "));
#endif // DS_CAP_SYS_LOG
  led.LowActive();            // ESP8266 builtin LED is active LOW
#ifdef DS_CAP_SYS_LOG
  log->println(F("OK"));
#endif // DS_CAP_SYS_LOG
#endif // DS_CAP_SYS_LED

#ifdef DS_CAP_BUTTON
// The code below assumes the button is active low
#ifdef DS_CAP_SYS_LOG
  log->printf(TIMED("Initializing button... "));
#endif // DS_CAP_SYS_LOG
  button.getButtonConfig()->setFeature(ButtonConfig::kFeatureLongPress);
  button.getButtonConfig()->setLongPressDelay(5000);    // 5s
  if (onButtonInit)
    onButtonInit();
  button.setEventHandler(buttonEventHandler);
  pinMode(button.getPin(), INPUT_PULLUP);   // External pull up works with this too
#ifdef DS_CAP_SYS_LOG
  log->println("OK");
#endif // DS_CAP_SYS_LOG
#endif // DS_CAP_BUTTON

#ifdef DS_CAP_SYS_TIME
  // Install time sync handler
  settimeofday_cb(timeSyncHandler);
#endif // DS_CAP_SYS_TIME

#ifdef DS_CAP_SYS_FS
#ifdef DS_CAP_SYS_LOG
  log->printf(TIMED("Mounting file system... "));
#endif // DS_CAP_SYS_LOG
  bool fs_ok = fs.begin();
#ifdef DS_CAP_SYS_LOG
  log->println(fs_ok ? F("OK") : F("FAILED"));
#endif // DS_CAP_SYS_LOG
#endif // DS_CAP_SYS_FS

#ifdef DS_CAP_APP_LOG
  bool app_log_ok = fs_ok;
#ifdef DS_CAP_SYS_LOG
  log->printf(TIMED("Starting application log... "));
#endif // DS_CAP_SYS_LOG
  if(app_log_ok && app_log_size_max) {
    FSInfo fsi;
    app_log_ok = fs.info(fsi);
    if (app_log_ok) {
      if (fsi.totalBytes > APP_LOG_SLACK) {
        if (fsi.totalBytes - APP_LOG_SLACK < app_log_size_max)
          app_log_size_max = fsi.totalBytes - APP_LOG_SLACK;
        app_log = fs.open(APP_LOG_FILE_NAME, "a");
        app_log_ok = app_log;
        if (app_log_ok) {
          app_log_size = app_log.size();
          if (fs.exists(APP_LOG_FILE_NAME2)) {
            auto app_log2 = fs.open(APP_LOG_FILE_NAME2, "r");
            app_log_ok = app_log2;
            if (app_log_ok) {
              app_log_size += app_log2.size();
              app_log2.close();
            } else
              app_log.close();
          }
        }
      } else
        app_log_ok = false;  // Not enough space for log
    }
  }
#ifdef DS_CAP_SYS_LOG
  log->println(app_log_ok ? (app_log_size_max ? F("OK") : F("DISABLED")): F("FAILED"));
#endif // DS_CAP_SYS_LOG
  if (!app_log_ok)
    app_log_size_max = 0;
#endif // DS_CAP_APP_LOG

#ifdef DS_CAP_SYS_NETWORK
  // Initialize network
  connectNetwork(
#ifdef DS_CAP_SYS_LED
    &led
#endif // DS_CAP_SYS_LED
    );
#endif // DS_CAP_SYS_NETWORK

#ifdef DS_CAP_MDNS
#ifdef DS_CAP_SYS_LOG
  log->printf(TIMED("Starting mDNS service for %s.local... "), hostname);
#endif // DS_CAP_SYS_LOG
  MDNS.begin(hostname);
#ifdef DS_CAP_SYS_LOG
  log->println(F("OK"));
#endif // DS_CAP_SYS_LOG
#endif // DS_CAP_MDNS

#ifdef DS_CAP_WEBSERVER
  // Start web service
#ifdef DS_CAP_SYS_LOG
  log->printf(TIMED("Starting web server... "));
#endif // DS_CAP_SYS_LOG
  web_page.reserve(MAX_WEB_PAGE_SIZE);
  web_server.on("/about", serveAbout);
#ifdef DS_CAP_APP_LOG
  web_server.on("/log", serveAppLog);
#endif // DS_CAP_APP_LOG
#ifdef DS_CAP_SYS_FS
  if (fs.exists(FAV_ICON_PATH))
    web_server.serveStatic(FAV_ICON_PATH, fs, FAV_ICON_PATH);
#endif // DS_CAP_SYS_FS
  if (registerWebPages)
    registerWebPages();
  web_server.begin();
#ifdef DS_CAP_SYS_LOG
  log->println(F("OK"));
#endif // DS_CAP_SYS_LOG
#endif // DS_CAP_WEBSERVER

#ifdef DS_CAP_SYS_LOG
  log->printf(TIMED("DS System v"));
  log->print(getVersion());
  log->print(F(" initialization completed. Configured capabilities: "));
  log->println(getCapabilities());
#endif // DS_CAP_SYS_LOG

}

// Update system
void System::update() {

#ifdef DS_CAP_APP_LOG
  if (app_log_size_max && app_log_size >= app_log_size_max) {
    bool rotation_ok = true;
#ifdef DS_CAP_SYS_LOG
    log->printf(TIMED("Max application log size (%zu) reached, rotating...\n"), app_log_size_max);
#endif // DS_CAP_SYS_LOG
    app_log_size = app_log.size();
    app_log.close();
    if (fs.exists(APP_LOG_FILE_NAME2))
      rotation_ok = fs.remove(APP_LOG_FILE_NAME2);     // Rename will fail if file exists
    if (rotation_ok) {
      rotation_ok = fs.rename(APP_LOG_FILE_NAME, APP_LOG_FILE_NAME2);
      if (rotation_ok) {
        app_log = fs.open(APP_LOG_FILE_NAME, "a");
        rotation_ok = app_log;
      }
    }
    if (!rotation_ok) {
      app_log_size_max = 0;
#ifdef DS_CAP_SYS_LOG
      log->printf(TIMED("Application log rotation failed; disabling logging\n"));
#endif // DS_CAP_SYS_LOG
    }
  }
#endif // DS_CAP_APP_LOG

#ifdef DS_CAP_SYS_LED
  led.Update();
#endif // DS_CAP_SYS_LED

#ifdef DS_CAP_BUTTON
  button.check();
#endif // DS_CAP_BUTTON

#ifdef DS_CAP_WIFIMANAGER
  // Check if network configuration is needed
  if (needsNetworkConfiguration()) {
#ifdef DS_CAP_WEBSERVER
    web_server.stop();
#endif // DS_CAP_WEBSERVER
    configureNetwork();
#ifdef DS_CAP_WEBSERVER
    web_server.begin();
#endif // DS_CAP_WEBSERVER
  }
#endif // DS_CAP_WIFIMANAGER

#ifdef DS_CAP_MDNS
  MDNS.update();
#endif // DS_CAP_MDNS

#ifdef DS_CAP_WEBSERVER
  web_server.handleClient();
#endif // DS_CAP_WEBSERVER

#ifdef DS_CAP_TIMERS_ABS
  static time_t old_time = 0;                  // Last second value
  const auto new_time = getTime();
  if (new_time != old_time) {   // Happens once a second
    old_time = new_time;
    struct tm tm_local;
    localtime_r(&new_time, &tm_local);

#ifdef DS_CAP_TIMERS_SOLAR
    static time_t time_solar_sync = 0;           // Last time the solar events have been calculated

    // Recalculate solar times every morning at 3:30 am, or at least every 24 hours
    if ((tm_local.tm_hour == 3 && tm_local.tm_min == 30 && tm_local.tm_sec == 0) || new_time - time_solar_sync > 24 * 60 * 60) {
      time_solar_sync = new_time;
      log->printf(TIMED("Recalculating solar events...\n"));
      const auto sunrise = getSunrise();
      const auto sunset  = getSunset();
      for (auto& timer : timers) {
        const auto timer_type = timer.getType();
        if (timer_type == TIMER_SUNRISE || timer_type == TIMER_SUNSET) {
          auto st = static_cast<TimerSolar *>(&timer);

           // This might not work properly with solar events happening shortly past midnight, but these are unlikely
          st->setHour(((timer_type == TIMER_SUNRISE ? sunrise : sunset) + st->getOffset()) / 60);
          st->setMinute(((timer_type == TIMER_SUNRISE ? sunrise : sunset) + st->getOffset()) % 60);
          st->setSecond(0);  // Calculation has minute precision
        }
      }
    }
#endif // DS_CAP_TIMERS_SOLAR

    // Process timers
    if (abs_timers_active)
      for (auto& timer : timers) {
        if (timer.getType() != TIMER_INVALID && timer.isArmed() && timer == tm_local) {
          log->printf(TIMED("Timer \"%s\" fired\n"), timer.getLabel().c_str());
          if (timerHandler)
            timerHandler(timer);
          if (!timer.isRecurrent())
            timer.disarm();
          if (timer.isTransient())
            timers.remove(timer);           // FIXME check for memory leak
        }
#ifdef DS_CAP_TIMERS_COUNT
        if (timer.getType() == TIMER_COUNTDOWN) {
          static_cast<TimerCountdown &>(timer).update(old_time);
        }
#endif // DS_CAP_TIMERS_COUNT
      }
  }
#endif // DS_CAP_TIMERS_ABS
}

// Return list of configured capabilities
String System::getCapabilities() {
  String capabilities;

#ifdef DS_CAP_APP_ID
  capabilities += F("APP_ID ");
#endif // DS_CAP_APP_ID

#ifdef DS_CAP_APP_LOG
  capabilities += F("APP_LOG ");
#endif // DS_CAP_APP_LOG

#ifdef DS_CAP_SYS_LED
  capabilities += F("SYS_LED ");
#endif // DS_CAP_SYS_LED

#ifdef DS_CAP_SYS_LOG
#ifdef DS_CAP_SYS_LOG_HW
  capabilities += F("SYS_LOG_HW ");
#else
  capabilities += F("SYS_LOG ");
#endif // DS_CAP_SYS_LOG_HW
#endif // DS_CAP_SYS_LOG

#ifdef DS_CAP_SYS_RESET
  capabilities += F("SYS_RESET ");
#endif // DS_CAP_SYS_RESET

#ifdef DS_CAP_SYS_RTCMEM
  capabilities += F("SYS_RTCMEM ");
#endif // DS_CAP_SYS_RTCMEM

#ifdef DS_CAP_SYS_TIME
  capabilities += F("SYS_TIME ");
#endif // DS_CAP_SYS_TIME

#ifdef DS_CAP_SYS_UPTIME
  capabilities += F("SYS_UPTIME ");
#endif // DS_CAP_SYS_TIME

#ifdef DS_CAP_SYS_FS
  capabilities += F("SYS_FS ");
#endif // DS_CAP_SYS_FS

#ifdef DS_CAP_SYS_NETWORK
  capabilities += F("SYS_NETWORK ");
#endif // DS_CAP_SYS_NETWORK

#ifdef DS_CAP_WIFIMANAGER
  capabilities += F("WIFIMANAGER ");
#endif // DS_CAP_WIFIMANAGER

#ifdef DS_CAP_MDNS
  capabilities += F("MDNS ");
#endif // DS_CAP_MDNS

#ifdef DS_CAP_WEBSERVER
  capabilities += F("WEBSERVER ");
#endif // DS_CAP_WEBSERVER

#ifdef DS_CAP_BUTTON
  capabilities += F("BUTTON ");
#endif // DS_CAP_BUTTON

#ifdef DS_CAP_TIMERS_ABS
  capabilities += F("TIMERS_ABS ");
#endif // DS_CAP_TIMERS_ABS

#ifdef DS_CAP_TIMERS_SOLAR
  capabilities += F("TIMERS_SOLAR ");
#endif // DS_CAP_TIMERS_SOLAR

#ifdef DS_CAP_TIMERS_COUNT
  capabilities += F("TIMERS_COUNT ");
#endif // DS_CAP_TIMERS_COUNT

#ifdef DS_CAP_WEB_TIMERS
  capabilities += F("WEB_TIMERS ");
#endif // DS_CAP_WEB_TIMERS

  capabilities.trim();
  return capabilities;
}

// Get system version
// Format is x.xx.xx (major.minor.maintenance). E.g., 20001 means 2.0.1
uint32_t System::getVersion() {
  return 10000;
}
