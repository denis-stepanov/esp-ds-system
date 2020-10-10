Library of reusable functions for rapid development of ESP8266 sketches. Helps avoiding copy-pasting supporting code from one sketch to another and focus straight away on sketch business.

Supported features (aka "system capabilities"):
-----------------------------------------------
* Application identification
* Application log with log browser and automatic log rotation
* Syslog on hardware serial line
* Builtin LED
* Builtin button (where available)
* Information on reset reason
* RTC memory
* System time, including support for time zone and sync via NTP
* Record of boot time; uptime counter
* File system (LittleFS or SPIFFS)
* Wi-Fi network
* Wi-Fi manager (configuration of Wi-Fi at runtime)
* mDNS
* Web server with pages template, system info page and favicon support
* ... add your own ...

Capabilities may have synergy; e.g., if you activate both 'time' and 'network', time will be automatically sync from NTP. And if you add a 'led' on top, Wi-Fi connection process will be signalled with LED.

Most features rely on excellent libraries provided on GitHub and make just a thin interface on top. Library objects are deliberately declared 'public', so you have full access to the native interface, should you need it. Few things I haven't found a good support and implemented myself:
* application logger with log rotation and decent web browsing;
* "about" web page listing many ESP8266 runtime details (memory, file system, time status, ...);

Usage:
------
1. Download the latest release locally (installation via Arduino Library Manager is not supported - [see why](https://github.com/denis-stepanov/esp-ds-system/wiki/Design));
2. Copy files System.h, System.cpp and MySystem.h from src/ into your sketch folder;
3. Uncomment the desired features in MySystem.h;
4. Add the following lines in your sketch (.ino):

		#include "MySystem.h"
		...

		void setup() {
	  		ds::System::begin();   // Could be omitted for some capabilities
	  	...
		}

		void loop() {
	  		ds::System::update();  // Could be omitted for some capabilities
	  	...
		}

Example: NTP support
--------------------

MySystem.h:

	#define DS_CAP_SYS_NETWORK  // Enable network
	#define DS_CAP_SYS_TIME     // Enable system time
	#define DS_CAP_SYS_LOG_HW   // Enable syslog on hardware serial line

	#define DS_TIMEZONE TZ_Europe_Paris  // Timezone. Pick yours from TZ.h coming with ESP Core

	#include "System.h"         // System global definitions

sketch.ino:

	#include "MySystem.h"

	// Set network parameters
	const char *ds::System::wifi_ssid = "mySSID";             // Network SSID
	const char *ds::System::wifi_pass = "myPassword";         // Network password

	void setup() {
	  ds::System::begin();
	  delay(2000);           // Allow time to synchronize
	  ds::System::log->print("Current time: ");
	  ds::System::log->println(ds::System::getTimeStr());  
	}

	void loop() {
	}

Output when run:

	0000000066: Started
	0000000067: Connecting to network 'mySSID'... connected. IP address: 192.168.1.2
	0000005969: Starting NTP client service... OK
	0000005970: DS System v10000 initialization completed. Configured capabilities: SYS_LOG_HW SYS_TIME SYS_NETWORK
	0000007974: System clock set: 2020/10/08 23:30:50
	Current time: 2020/10/08 23:30:50

For detailed information and examples see [Wiki pages](https://github.com/denis-stepanov/esp-ds-system/wiki/).
