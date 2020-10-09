// RTC memory example
// Board: NodeMCU 1.0(ESP-12E Module)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

void setup() {
  System::begin();

  // Send one number (four bytes) into RTC memory
  uint32_t number_to_write = 3;
  auto ret = System::setRTCMem(&number_to_write);
  System::log->printf("Writing to RTC memory: num=%u, status=%s\n", number_to_write, ret ? "SUCCESS" : "FAILURE");

  // Normally here one would go to deep sleep
  // ...

  // Fetch stored number from RTC memory
  uint32_t restored_number;
  ret = System::getRTCMem(&restored_number);
  System::log->printf("Reading from RTC memory: num=%u, status=%s\n", restored_number, ret ? "SUCCESS" : "FAILURE");

  // Example of storing an array with an offset of 10 slots (i.e., starting from 40th byte of RTC)
  uint32_t data_to_write[3] = {3, 14, 159};
  ret = System::setRTCMem(data_to_write, 10, 3);
  System::log->printf("Writing to RTC memory: nums=%u %u %u, status=%s\n", data_to_write[0], data_to_write[1], data_to_write[2], ret ? "SUCCESS" : "FAILURE");
  uint32_t restored_data[3];
  ret = System::getRTCMem(restored_data, 10, 3);
  System::log->printf("Reading from RTC memory: nums=%u %u %u, status=%s\n", restored_data[0], restored_data[1], restored_data[2], ret ? "SUCCESS" : "FAILURE");
}

void loop() {
}
