// File system example
// Board: NodeMCU 1.0(ESP-12E Module)
// Be sure to enable file system in Arduino settings (Tools -> Flash Size)
// (!) Before compiling, copy System.h and System.cpp into the sketch folder, then reopen the sketch in Arduino

#include "MySystem.h"

using namespace ds;

const char *file_name PROGMEM = "/ds-system-fs-example.txt";     // Sample file name

void setup() {
  System::begin();

  // Check file system availability
  FSInfo fsi;
  if(System::fs.info(fsi))
    System::log->printf("File system usage: %zu / %zu bytes\n", fsi.usedBytes, fsi. totalBytes);
  else {
    System::log->println("No file system found, or file system not formatted");
    return;    
  }

  // Delete sample file (OK if not exists)
  System::fs.remove(file_name);

  // Create new file
  File my_file = System::fs.open(file_name, "w");
  if(!my_file) {
    System::log->println("File creation error");
    return;        
  }
  my_file.println("Hello, world!");
  my_file.close();
  System::log->printf("Wrote file %s\n", file_name);

  // Read the file back
  my_file = System::fs.open(file_name, "r");
  String line = my_file.readStringUntil('\n');
  my_file.close();
  System::log->println("Reading from file:");
  System::log->println(line);
}

void loop() {
}
