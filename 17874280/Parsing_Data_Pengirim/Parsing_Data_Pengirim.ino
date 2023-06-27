//-------------------OLED-----
#include <SPI.h>
#include <Wire.h>

//--------------------------------
#include <SoftwareSerial.h>

float Celcius=0;
float Fahrenheit=0;

const int buttonPin = 5;     // the number of the pushbutton pin

String str;

SoftwareSerial mySerial(11, 10); // RX, TX

void setup() {

   Serial.begin(9600);
   mySerial.begin(9600);
}

void loop() {

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = 10;
  // Read temperature as Celsius (the default)
  float t = 20;
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = 10;

  Serial.println("_______________________");
  str =String('*')+String(h)+String(',')+String(t)+String('#');
  mySerial.println(str);
}
