#include "Lock.h"
#define ORANGE 3
Lock test;

void setup() {  
  
  Serial.begin(9600);
  test.INIT();
  test.RFID_setup();
}

void loop() {
   test.button();
   test.RFID_read();
   test.on_lock();
   //test.GPS_periodic();
   test.LED();
}
