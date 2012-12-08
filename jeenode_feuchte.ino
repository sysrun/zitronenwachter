/// @dir lux_demo
/// Demo of the Lux Plug, based on the LuxPlug class in the Ports library.
// 2010-03-18 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <JeeLib.h>
#define DEBUG true
    
Port sensor1 (3);
MilliTimer report;

struct {
    uint16_t sensor1;     // light sensor
    uint16_t sensor2;  // motion detector
    byte lobat :8;  // supply voltage dropped under 3.1V
} payload;

// has to be defined because we're using the watchdog for low-power waiting
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

void setup () {
    #if DEBUG
    Serial.begin(57600);
    Serial.println("\n[jeenode_feuchte]");
    #endif
    rf12_initialize(11, RF12_868MHZ, 212);
}
int wert1;
void loop () {
  wert1 = sensor1.anaRead();
  if (report.poll(1000)) {
    #if DEBUG
      Serial.println(wert1);
    #endif
    payload.sensor1 = wert1;
    payload.sensor2 = wert1;
    payload.lobat = rf12_lowbat();

    rf12_sleep(RF12_WAKEUP);
    while (!rf12_canSend())
      rf12_recvDone();
    rf12_sendStart(0, &payload, sizeof payload, 2);
    rf12_sleep(RF12_SLEEP);
  }
}
