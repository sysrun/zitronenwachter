/// @dir lux_demo
/// Demo of the Lux Plug, based on the LuxPlug class in the Ports library.
// 2010-03-18 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <JeeLib.h>
//#define DEBUG false
#define NODEID 11
#define GROUP 212
Port sensor1 (3);
DHTxx dht (4); // PORT1,D

MilliTimer report;
MilliTimer dhtRead;

struct {
    uint16_t sensor1;     // light sensor
    uint16_t sensor2;  // motion detector
    uint16_t temp;
    uint16_t humi;
    byte lobat :8;  // supply voltage dropped under 3.1V
} payload;

// has to be defined because we're using the watchdog for low-power waiting
//ISR(WDT_vect) { Sleepy::watchdogEvent(); }

void setup () {
    #if DEBUG
    Serial.begin(57600);
    Serial.println("\n[jeenode_feuchte]");
    #endif
    rf12_initialize(NODEID, RF12_868MHZ, GROUP);
}
int wert1;
int t,h;
void loop () {
  wert1 = sensor1.anaRead();
  
  // Read DHT11 Sensor
  if (dhtRead.poll(60000 || t == 0)) {
    if (dht.reading(t, h)) {
      payload.temp = t;
      payload.humi = h;
    }
  }
  
  // Send Payload
  if (report.poll(30000)) {
    #if DEBUG
      Serial.print(wert1);
      Serial.print(" - T:");
      Serial.print((payload.temp/10));
      Serial.print(" - H:");
      Serial.print((payload.humi/10));
      Serial.print("%");
      Serial.println();
    #endif
    payload.sensor1 = wert1;
    payload.sensor2 = wert1;
    payload.lobat = rf12_lowbat();

    rf12_sleep(RF12_WAKEUP);
    while (!rf12_canSend())
      rf12_recvDone();
    rf12_sendStart(0, &payload, sizeof payload);
    delay(10);
    rf12_sleep(RF12_SLEEP);
  }
}
