#define THINGER_SERIAL_DEBUG
#include <ThingerESP32.h>
#include "arduino_secrets.h"

ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

void setup_thingerIO() {

  thing.add_wifi(SSID, SSID_PASSWORD);

  thing["CO2"] >> outputValue(co2ppm);
  thing["CO"] >> outputValue(coppm);
  thing["NH3"] >> outputValue(nh3ppm);
  thing["AQI"] >> outputValue(AQI);
  thing["hPa"] >> outputValue(hPa);
  thing["Altitude"] >> outputValue(altitude);
  thing["temp"] >> outputValue(temp);
  thing["hum"] >> outputValue(hum);

  // resource output example (i.e. reading a sensor value)
  //thing["millis"] >> outputValue(millis());

  // more details at http://docs.thinger.io/arduino/
}

void send_thingerIO() {
  thing.handle();
}