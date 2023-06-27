#include "AQI.h"
#include "softwareSerial.h"
#include "thingerIO.h"
#include "arduino_secrets.h"

// Entry point for the example
void setup()
{
  setup_AQI();
  setupSerial();
  setup_thingerIO();

  /* Initializes the Serial communication */
  Serial.begin(9600);
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);
}

// Function that is looped forever
void loop()
{
  read_AQI();
  recParsingData();
  send_thingerIO();
}




