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

int potPin = A7;
int GNDpin = A0;
int Vpin = A1;

/**************************************************************************/

void setup() {
  // put your setup code here, to run once:

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
  // put your main code here, to run repeatedly:

  /* read signal from input on arduino nano */
  int sensorVal = map(analogRead(potPin), 0, 1023, 0, 5000);
  int gndVal = map(analogRead(GNDpin), 0, 1023, 0, 5000);
  int vVal = map(analogRead(Vpin), 0, 1023, 0, 5000);

  String data = String(sensorVal) + " " + String(gndVal) + " " + String(vVal);

  /* send to bluefruit */
  ble.println(data);

  // check response status
  if (! ble.waitForOK() ) {
    Serial.println(F("Failed to send?"));
  }

  delay(200);
}