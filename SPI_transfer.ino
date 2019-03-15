#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BluefruitLE_SPI.h"
#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

#define FACTORYRESET_ENABLE         0  // change to 0 when deploying project. allows the module to bond with ipad
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"

// Create the bluefruit object,
// hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

int Kpin = A7;
int Gpin = A0;
int Lpin = A1;

/**************************************************************************/

void setup() {

  Serial.begin(115200);

  /* Initialise the module */
  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  /* Change the device name to make it easier to find */
  if (! ble.sendCommandCheckOK(F( "AT+GAPDEVNAME=Aishwarya's Bluefruit" )) ) {
    error(F("Could not set device name?"));
  }

  /* Set module to DATA mode */
  ble.setMode(BLUEFRUIT_MODE_DATA);
  
}


/**************************************************************************/

void loop() {

  // read signal from potassium pin and send
  unsigned long timeK = millis();
  int Kval = map(analogRead(Kpin), 0, 1023, 0, 5000);
  String Koutput = "K" + String(timeK) + " " + String(Kval);
  ble.print(Koutput);
  delay(200);

  // read signal from glucose pin and send
  unsigned long timeG = millis();
  int Gval = map(analogRead(Gpin), 0, 1023, 0, 5000);
  String Goutput = "G" + String(timeG) + " " + String(Gval);
  ble.print(Goutput);
  delay(200);

  // read signal from lactate pin and send
  unsigned long timeL = millis();
  int Lval = map(analogRead(Lpin), 0, 1023, 0, 5000);
  String Loutput = "L" + String(timeG) + " " + String(Lval);
  ble.print(Loutput); 

  delay(600);
}
