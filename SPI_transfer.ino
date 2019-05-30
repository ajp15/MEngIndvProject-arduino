#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BluefruitLE_SPI.h"
#include "BluefruitConfig.h"
#include <Filters.h>


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
int i = 1;
int Ksum = 0;
int Gsum = 0;
int Lsum = 0;
int Kcount = 0;
int Gcount = 0;
int Lcount = 0;

float filterfreq = 1.0;
FilterOnePole lowpassFilterK(LOWPASS, filterfreq);
FilterOnePole lowpassFilterG(LOWPASS, filterfreq);
FilterOnePole lowpassFilterL(LOWPASS, filterfreq);

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

  unsigned long T = millis();

  // read input signals and store in array
  int K = lowpassFilterK.input(analogRead(Kpin));
  int Kval = map(K, 0, 1023, 0, 5000);
  Ksum = Ksum + Kval;
  Kcount = Kcount + 1;
  /*Serial.println("K");
  Serial.println(Kcount);
  Serial.println(Ksum);*/

  int G = lowpassFilterG.input(analogRead(Gpin));
  int Gval = map(G, 0, 1023, 0, 5000);
  Gsum = Gsum + Gval;
  Gcount = Gcount + 1;
  /*Serial.println("G");
  Serial.println(Gcount);
  Serial.println(Gsum);*/

  int L = lowpassFilterL.input(analogRead(Lpin));
  int Lval = map(L, 0, 1023, 0, 5000);
  Lsum = Lsum + Lval;
  Lcount = Lcount + 1;
  /*Serial.println("L");
  Serial.println(Lcount);
  Serial.println(Lsum);*/

  if (i == 1) {

    // send K data
    int avgK = Ksum/Kcount;
    String Koutput = "K" + String(T) + " " + String(avgK) + "\n";
    ble.print(Koutput);
    //Serial.println(Koutput);

    // reset variables
    Ksum = 0;
    Kcount = 0;
    
  } 
  else if (i == 3) {

    // send G data
    int avgG = Gsum/Gcount;
    String Goutput = "G" + String(T) + " " + String(avgG) + "\n";
    ble.print(Goutput);
    //Serial.println(Goutput);

    // reset variables
    Gsum = 0;
    Gcount = 0;
    
  }
  else if (i == 5) {

    // send L data
    int avgL = Lsum/Lcount;
    String Loutput = "L" + String(T) + " " + String(avgL) + "\n";
    ble.print(Loutput);
    //Serial.println(Loutput);

    // reset variables
    Lsum = 0;
    Lcount = 0;
    // reset iterator
    i = 0;
  }

  i = i + 1;
  delay(200);
}
