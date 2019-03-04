#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BluefruitLE_SPI.h"
#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

#define FACTORYRESET_ENABLE         1  // change to 0 when deploying project
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "SPI" //LED activity, valid options are "DISABLE" or "MODE" or "BLEUART" or "HWUART"  or "SPI"  or "MANUAL"

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

 // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  } 
  
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
  ble.print("AT+BLEUARTTX=" data);
  ble.println(data);

  // check response status
  if (! ble.waitForOK() ) {
    Serial.println(F("Failed to send?"));
  }

  delay(2000);
}
