//Heart Rate Sensor Setting ----
unsigned long previousMillisGetHR = 0;     //--> will store the last time Millis (to get Heartbeat) was updated.
unsigned long previousMillisResultHR = 0;  //--> will store the last time Millis (to get BPM) was updated.

const long intervalGetHR = 1;         //--> Interval for reading heart rate (Heartbeat) = 10ms.
const long intervalResultHR = 10000;  //--> The reading interval for the result of the Heart Rate calculation is in 10 seconds.

int PulseSensorSignal;             //--> Variable to accommodate the signal value from the sensor
const int PulseSensorHRWire = 34;  //--> PulseSensor connected to ANALOG PIN 0 (A0 / ADC 0).
//const int LED_A1 = A1;            //--> LED to detect when the heart is beating. The LED is connected to PIN A1 on the Arduino UNO.
int UpperThreshold = 550;  //--> Determine which Signal to "count as a beat", and which to ingore.
int LowerThreshold = 500;

int cntHB = 0;                 //--> Variable for counting the number of heartbeats.
boolean ThresholdStat = true;  //--> Variable for triggers in calculating heartbeats.
int BPMval = 0;                //--> Variable to hold the result of heartbeats calculation.

int x = 0;      //--> Variable axis x graph values to display on OLED
int y = 0;      //--> Variable axis y graph values to display on OLED
int lastx = 0;  //--> The graph's last x axis variable value to display on the OLED
int lasty = 0;  //--> The graph's last y axis variable value to display on the OLED
int n = 0;
//------------------------------


//----------------------------------------'Heart_Icon', 16x16px
// I drew this heart icon at : http://dotmatrixtool.com/
const unsigned char Heart_Icon[] PROGMEM = {
  0x00, 0x00, 0x18, 0x30, 0x3c, 0x78, 0x7e, 0xfc, 0xff, 0xfe, 0xff, 0xfe, 0xee, 0xee, 0xd5, 0x56,
  0x7b, 0xbc, 0x3f, 0xf8, 0x1f, 0xf0, 0x0f, 0xe0, 0x07, 0xc0, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00
};
//----------------------------------------

//BH1750 Setting----------------
#include <BH1750.h>
#include <Wire.h>

BH1750 lightMeter;
//------------------------------

//MLX906 Setting---------------
#include <Adafruit_MLX90614.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
double temp_amb;
double temp_obj;

//----------------------------

//Software debouncing in Interrupt, by Delphi�o K.M.
long debouncing_time = 200;  //Debouncing Time in Milliseconds
volatile unsigned long last_micros;

//BME280 Setting
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;  // I2C

float temp, hum, press;
int alt;
//------------------------------

//Button Setting 2 4 26 27
const int bUp = 2;     //Button Up
const int bRight = 4;  //Button Right
const int bLeft = 26;  //Button Left
const int bDown = 27;  //Button Down

int bUpState, bRightState, bLeftState, bDownState;
int button = 1;
//-----------------------------

//GPS Setting
#include <TinyGPS++.h>
#include <TimeLib.h>
HardwareSerial neogps(1);
#define time_offset 28800  // define a clock offset of 3600 seconds (1 hour) ==> UTC + 8
TinyGPSPlus gps;
char Time[] = "TIME: 00:00:00";
char Date[] = "DATE: 00-00-2000";
byte last_second, Second, Minute, Hour, Day, Month, wday;
int Year, satellites2, altimeter2;
float latitude2, longitude2;
//-----------------------------

//SSD1306 OLED Setting
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/*#include <U8g2lib.h>
// all display types, add more if required, give them explicit names, then add them to the OLEDS 'display' array
U8G2_SSD1306_64X48_ER_F_HW_I2C         display64x48 (U8G2_R0, U8X8_PIN_NONE);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C    u8g2(U8G2_R0, U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C display128x32(U8G2_R0, U8X8_PIN_NONE);

//----------------------------*/

//i2c Setting
#include <Wire.h>
#define I2C_SDA 15
#define I2C_SCL 13
//-----------------------------

//Software Serial Setting
#define RXD2 16
#define TXD2 17
//-----------------------------

// Select I2C BUS
void TCA9548A(uint8_t bus) {
  Wire.beginTransmission(0x70);  // TCA9548A address
  Wire.write(1 << bus);          // send byte to select bus
  Wire.endTransmission();
  //Serial.print(bus);
}


void IRAM_ATTR ButtonRight() {
  if ((long)(micros() - last_micros) >= debouncing_time * 1000) {
    button++;
    if (button > 8) {
      button = 1;
    }
    last_micros = micros();
    Serial.println(button);
  }
}

void IRAM_ATTR ButtonDown() {
  if ((long)(micros() - last_micros) >= debouncing_time * 1000) {
    button--;
    if (button < 0) {
      button = 8;
    }
    last_micros = micros();
    Serial.println(button);
  }
}

//Prayer Times Setting -----------
#include <stdio.h>
#include <PrayerTimes.h>

double times[sizeof(TimeName) / sizeof(char *)];

void p(char *fmt, ...) {
  char tmp[128];  // resulting string limited to 128 chars
  va_list args;
  va_start(args, fmt);
  vsnprintf(tmp, 128, fmt, args);
  va_end(args);
}
//--------------------------------


void setup() {
  //Button Setting
  pinMode(bUp, INPUT_PULLUP);
  pinMode(bRight, INPUT_PULLUP);
  pinMode(bLeft, INPUT_PULLUP);
  pinMode(bDown, INPUT_PULLUP);

  attachInterrupt(bRight, ButtonRight, FALLING);  
  attachInterrupt(bDown, ButtonDown, FALLING);

  //i2c Setting
  Wire.begin(I2C_SDA, I2C_SCL);

  //Begin serial communication Arduino IDE (Serial Monitor)
  Serial.begin(9600);

  //Begin serial communication Neo6mGPS
  neogps.begin(9600, SERIAL_8N1, RXD2, TXD2);

  //Init OLED
  TCA9548A(2);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  // Clear the buffer
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print("MuSeDev");
  display.display();
  delay(3000);

  display.clearDisplay();

  //Init BME280
  TCA9548A(5);
  bool status5;
  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status5 = bme.begin(0x76);
  if (!status5) {
    Serial.println("Could not detect a BME280 sensor, Fix wiring Connections!");
    while (1)
      ;
  }

  //Init BH1750
  TCA9548A(3);
  lightMeter.begin();

  Serial.println(F("BH1750 Test begin"));

  //Init MLX906
  TCA9548A(6);
  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1)
      ;
  };
}

void heartRate() {
  TCA9548A(2);
  while (n == 0) {
    //----------------------------------------Displays BPM value reading information
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    display.setCursor(0, 12);  //--> (x position, y position)
    display.print("     Please wait");

    display.setCursor(0, 22);  //--> (x position, y position)
    display.print("     10  seconds");

    display.setCursor(0, 32);  //--> (x position, y position)
    display.print("       to get");

    display.setCursor(0, 42);  //--> (x position, y position)
    display.print(" the Heart Rate value");

    display.display();
    delay(3000);
    //----------------------------------------

    //----------------------------------------Displays the initial display of BPM value
    display.clearDisplay();                                //--> for Clearing the display
    display.drawBitmap(0, 47, Heart_Icon, 16, 16, WHITE);  //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)

    display.drawLine(0, 43, 127, 43, WHITE);  //--> drawLine(x1, y1, x2, y2, color)

    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(20, 48);  //--> (x position, y position)
    display.print(": 0 BPM");
    display.display();
    //----------------------------------------

    Serial.println();
    Serial.println("Please wait 10 seconds to get the BPM Value");
    n = 1;
  }

  GetHeartRate();
}

//--------------------------------------------------------------------------------void GetHeartRate()
// This subroutine is for reading the heart rate and calculating it to get the BPM value.
// To get a BPM value based on a heart rate reading for 10 seconds.
void GetHeartRate() {
  //----------------------------------------Process of reading heart rate.
  unsigned long currentMillisGetHR = millis();

  if (currentMillisGetHR - previousMillisGetHR >= intervalGetHR) {
    previousMillisGetHR = currentMillisGetHR;

    PulseSensorSignal = analogRead(PulseSensorHRWire);  //--> holds the incoming raw data. Signal value can range from 0-1024

    if (PulseSensorSignal > UpperThreshold && ThresholdStat == true) {
      cntHB++;
      ThresholdStat = false;
      //digitalWrite(LED_A1,HIGH);
    }

    if (PulseSensorSignal < LowerThreshold) {
      ThresholdStat = true;
      //digitalWrite(LED_A1,LOW);
    }

    DrawGraph();  //--> Calling the DrawGraph() subroutine
  }
  //----------------------------------------

  //----------------------------------------The process for getting the BPM value.
  unsigned long currentMillisResultHR = millis();

  if (currentMillisResultHR - previousMillisResultHR >= intervalResultHR) {
    previousMillisResultHR = currentMillisResultHR;

    BPMval = cntHB * 6;  //--> The taken heart rate is for 10 seconds. So to get the BPM value, the total heart rate in 10 seconds x 6.
    Serial.print("BPM : ");
    Serial.println(BPMval);

    display.fillRect(20, 48, 108, 18, BLACK);

    display.drawBitmap(0, 47, Heart_Icon, 16, 16, WHITE);  //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
    display.drawLine(0, 43, 127, 43, WHITE);               //--> drawLine(x1, y1, x2, y2, color)

    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(20, 48);  //--> (x position, y position)
    display.print(": ");
    display.print(BPMval);
    display.print(" BPM");
    display.display();

    cntHB = 0;
  }
  //----------------------------------------
}
//--------------------------------------------------------------------------------

//----Subroutines for drawing or displaying heart rate graphic signals
void DrawGraph() {
  //----------------------------------------Condition to reset the graphic display if it fills the width of the OLED screen
  if (x > 127) {
    display.fillRect(0, 0, 128, 42, BLACK);
    x = 0;
    lastx = 0;
  }
  //----------------------------------------

  //----------------------------------------Process signal data to be displayed on OLED in graphic form
  int ySignal = PulseSensorSignal;

  if (ySignal > 850) ySignal = 850;
  if (ySignal < 350) ySignal = 350;

  int ySignalMap = map(ySignal, 350, 850, 0, 40);  //--> The y-axis used on OLEDs is from 0 to 40

  y = 40 - ySignalMap;
  //----------------------------------------

  //----------------------------------------Displays the heart rate graph
  display.writeLine(lastx, lasty, x, y, WHITE);
  display.display();
  //----------------------------------------

  lastx = x;
  lasty = y;

  x++;
}
//--------------------------------------------------------------------------------

void BME280_1() {
  TCA9548A(5);
  temp = bme.readTemperature();
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");

  // Convert temperature to Fahrenheit
  /*Serial.print("Temperature = ");
  Serial.print(1.8 * bme.readTemperature() + 32);
  Serial.println(" *F");*/

  press = bme.readPressure() / 100.0F;
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  hum = bme.readHumidity();
  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
  display.setCursor(0, 0);
  display.clearDisplay();

  TCA9548A(2);
  //display.print("Temperature: "); display.print(temp); display.println("*C");
  //display.print("Humidity: "); display.print(hum); display.println(" %");
  //display.print("Pressure: "); display.print(press); display.println(" hPa");
  // display temperature
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temperature: ");
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(temp);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(2);
  display.print("C");

  // display humidity
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Humidity: ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print(hum);
  display.print(" %");
  display.display();
  delay(1000);
}

void BME280_2() {
  TCA9548A(5);
  temp = bme.readTemperature();
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");

  // Convert temperature to Fahrenheit
  /*Serial.print("Temperature = ");
  Serial.print(1.8 * bme.readTemperature() + 32);
  Serial.println(" *F");*/

  press = bme.readPressure() / 100.0F;
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  hum = bme.readHumidity();
  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
  display.setCursor(0, 0);
  display.clearDisplay();

  TCA9548A(2);
  //display.print("Temperature: "); display.print(temp); display.println("*C");
  //display.print("Humidity: "); display.print(hum); display.println(" %");
  //display.print("Pressure: "); display.print(press); display.println(" hPa");
  // display temperature
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Pressure: ");
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(press);
  display.print(" ");
  display.setCursor(90, 20);
  display.setTextSize(1);
  display.print("hPa");

  // display humidity
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Approx. Altitude: ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print(alt);
  display.print("  meter");
  display.display();
  delay(1000);
}

//Time and Date Scan-----------------
void timedate() {
  while (neogps.available() > 0) {
    if (gps.encode(neogps.read())) {
      // get time from GPS module
      if (gps.time.isValid()) {
        Minute = gps.time.minute();
        Second = gps.time.second();
        Hour = gps.time.hour();
      } else {
        TCA9548A(2);
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.setTextSize(2);
        display.print("Time&Date");

        display.setCursor(0, 20);
        display.setTextSize(1);
        display.print("Positioning");
        display.display();

        delay(100);
        display.clearDisplay();

        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.setTextSize(2);
        display.print("Time&Date");

        display.setCursor(0, 20);
        display.setTextSize(1);
        display.print("Positioning.");
        display.display();

        delay(100);
        display.clearDisplay();

        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.setTextSize(2);
        display.print("Time&Date");

        display.setCursor(0, 20);
        display.setTextSize(1);
        display.print("Positioning..");
        display.display();

        delay(100);
        display.clearDisplay();

        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.setTextSize(2);
        display.print("Time&Date");

        display.setCursor(0, 20);
        display.setTextSize(1);
        display.print("Positioning...");
        display.display();
        delay(100);
      }

      // get date drom GPS module
      if (gps.date.isValid()) {
        Day = gps.date.day();
        Month = gps.date.month();
        Year = gps.date.year();
      }

      if (last_second != gps.time.second())  // if time has changed
      {
        last_second = gps.time.second();

        // set current UTC time
        setTime(Hour, Minute, Second, Day, Month, Year);
        // add the offset to get local time
        adjustTime(time_offset);

        // update time array
        Time[12] = second() / 10 + '0';
        Time[13] = second() % 10 + '0';
        Time[9] = minute() / 10 + '0';
        Time[10] = minute() % 10 + '0';
        Time[6] = hour() / 10 + '0';
        Time[7] = hour() % 10 + '0';

        // update date array
        Date[14] = (year() / 10) % 10 + '0';
        Date[15] = year() % 10 + '0';
        Date[9] = month() / 10 + '0';
        Date[10] = month() % 10 + '0';
        Date[6] = day() / 10 + '0';
        Date[7] = day() % 10 + '0';

        // print time & date
        print_wday(weekday());  // print day of the week
        Serial.println(Time);   // print time (HH:MM:SS)
        Serial.println(Date);   // print date (DD-MM-YYYY)
        TCA9548A(2);
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(75, 0);
        display.println(Time[12]);

        display.setTextSize(2);
        display.setCursor(87, 0);
        display.println(Time[13]);

        display.setTextSize(2);
        display.setCursor(25, 0);
        display.println(":");

        display.setTextSize(2);
        display.setCursor(65, 0);
        display.println(":");

        display.setTextSize(2);
        display.setCursor(40, 0);
        display.println(Time[9]);

        display.setTextSize(2);
        display.setCursor(52, 0);
        display.println(Time[10]);

        display.setTextSize(2);
        display.setCursor(0, 0);
        display.println(Time[6]);

        display.setTextSize(2);
        display.setCursor(12, 0);
        display.println(Time[7]);

        display.setTextSize(2);
        display.setCursor(0, 20);
        display.println(Date[6]);

        display.setTextSize(2);
        display.setCursor(12, 20);
        display.println(Date[7]);

        display.setTextSize(2);
        display.setCursor(30, 20);
        display.println("-");

        display.setTextSize(2);
        display.setCursor(45, 20);
        display.println(Date[9]);

        display.setTextSize(2);
        display.setCursor(57, 20);
        display.println(Date[10]);

        display.setTextSize(2);
        display.setCursor(67, 20);
        display.println("-");

        display.setTextSize(2);
        display.setCursor(82, 20);
        display.println(Date[14]);

        display.setTextSize(2);
        display.setCursor(94, 20);
        display.println(Date[15]);

        display.setTextSize(2);
        display.setCursor(0, 40);
        //display.print(wday);
        display.display();
      }
    }
  }
}

//GPS Scan-----------------
void GPS() {
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 1000;) {
    while (neogps.available()) {
      //Location
      if (gps.encode(neogps.read())) {
        newData = true;
      }
    }
  }

  //If newData is true
  if (newData == true) {
    newData = false;
    //Serial.println(gps.satellites.value());
    GPS_print();
  } else {
    TCA9548A(2);
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(3);
    display.print("No Data");
    display.display();
  }
}
void GPS_print() {
  TCA9548A(2);
  display.clearDisplay();
  display.setTextColor(WHITE);

  if (gps.location.isValid() == 1) {
    Serial.println("GPS");
    //String gps_speed = String(gps.speed.kmph());
    display.setTextSize(1);

    display.setCursor(25, 5);
    display.print("Lat: ");
    display.setCursor(50, 5);
    display.print(gps.location.lat(), 6);

    display.setCursor(25, 20);
    display.print("Lng: ");
    display.setCursor(50, 20);
    display.print(gps.location.lng(), 6);

    display.setCursor(0, 35);
    display.print("Speed(km/h): ");
    display.setCursor(85, 35);
    display.print(gps.speed.kmph());

    display.setTextSize(1);
    display.setCursor(0, 50);
    display.print("SAT:");
    display.setCursor(25, 50);
    display.print(gps.satellites.value());

    display.setTextSize(1);
    display.setCursor(70, 50);
    display.print("ALT:");
    display.setCursor(95, 50);
    display.print(gps.altitude.meters(), 0);

    display.display();
  } else {
    TCA9548A(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print("GPS");

    display.setCursor(0, 20);
    display.setTextSize(1);
    display.print("Positioning");
    display.display();

    delay(100);
    display.clearDisplay();

    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print("GPS");

    display.setCursor(0, 20);
    display.setTextSize(1);
    display.print("Positioning.");
    display.display();

    delay(100);
    display.clearDisplay();

    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print("GPS");

    display.setCursor(0, 20);
    display.setTextSize(1);
    display.print("Positioning..");
    display.display();

    delay(100);
    display.clearDisplay();

    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print("GPS");

    display.setCursor(0, 20);
    display.setTextSize(1);
    display.print("Positioning...");
    display.display();
    delay(100);
  }
}
//-------------------------

//Speedometer
void speedometer() {
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 1000;) {
    while (neogps.available()) {
      //Location
      if (gps.encode(neogps.read())) {
        newData = true;
      }
    }
  }

  //If newData is true
  if (newData == true) {
    newData = false;
    //Serial.println(gps.satellites.value());
    speedometer_print();
  } else {
    TCA9548A(2);
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(3);
    display.print("No Data");
    display.display();
  }
}

void speedometer_print() {
  TCA9548A(2);
  display.clearDisplay();
  display.setTextColor(WHITE);

  if (gps.location.isValid() == 1) {
    Serial.println("Speed");
    //String gps_speed = String(gps.speed.kmph());
    display.setCursor(0, 0);
    display.setTextSize(3);
    display.print(gps.speed.kmph());

    display.setCursor(75, 20);
    display.setTextSize(2);
    display.print("km/h");

    display.setTextSize(1);
    display.setCursor(0, 50);
    display.print("SAT:");
    display.setCursor(25, 50);
    display.print(gps.satellites.value());

    display.setTextSize(1);
    display.setCursor(70, 50);
    display.print("ALT:");
    display.setCursor(95, 50);
    display.print(gps.altitude.meters(), 0);

    display.display();
    delay(1000);
    display.clearDisplay();
  } else {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print("Speedomtr");

    display.setCursor(0, 20);
    display.setTextSize(1);
    display.print("Positioning");
    display.display();

    delay(100);
    display.clearDisplay();

    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print("Speedomtr");

    display.setCursor(0, 20);
    display.setTextSize(1);
    display.print("Positioning.");
    display.display();

    delay(100);
    display.clearDisplay();

    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print("Speedomtr");

    display.setCursor(0, 20);
    display.setTextSize(1);
    display.print("Positioning..");
    display.display();

    delay(100);
    display.clearDisplay();

    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print("Speedomtr");

    display.setCursor(0, 20);
    display.setTextSize(1);
    display.print("Positioning...");
    display.display();
    delay(100);
  }
}

// function for displaying day of the week
void print_wday(byte wday) {
  switch (wday) {
    case 1: Serial.println(" SUNDAY  "); break;
    case 2: Serial.println(" MONDAY  "); break;
    case 3: Serial.println(" TUESDAY "); break;
    case 4: Serial.println("WEDNESDAY"); break;
    case 5: Serial.println("THURSDAY "); break;
    case 6: Serial.println(" FRIDAY  "); break;
    default: Serial.println("SATURDAY ");
  }
}
//-------------------------

//BH1750-------------------
void BH1750() {
  TCA9548A(2);
  display.clearDisplay();
  TCA9548A(3);
  // Wait for completion (blocking busy-wait delay)
  float lux = lightMeter.readLightLevel();
  delay(1000);
  TCA9548A(2);
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print("Light");
  display.setCursor(58, 6);
  display.setTextSize(1);
  display.print(" Intensity:");
  display.setTextSize(2);
  display.print(lux);
  display.setCursor(90, 30);
  display.setTextSize(1);
  display.print(" LUX");
  display.display();
  /*
    display.setCursor(90,20);
    display.setTextSize(1);
    display.print("LUX");

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 35);
    display.print("LUX Meter: ");

    display.setCursor(x_LUX_val, 50); 
    display.print(result_lux);
    display.print(".");
    display.print(lux_HR);
    display.print(" LUX");
    display.display();

    display.setTextSize(1);
    display.setCursor(x_LUX_val, 50); 
    display.print(result_lux);
    display.print(".");
    display.print(lux_HR);
    display.print(" LUX");
    display.display();*/
}

//MLX906-------------------
void MLX1() {
  TCA9548A(6);

  Serial.print("Ambient temperature = ");
  Serial.print(mlx.readAmbientTempC());
  Serial.print("�C");
  Serial.print("   ");
  Serial.print("Object temperature = ");
  Serial.print(mlx.readObjectTempC());
  Serial.println("�C");

  Serial.print("Ambient temperature = ");
  Serial.print(mlx.readAmbientTempF());
  Serial.print("�F");
  Serial.print("   ");
  Serial.print("Object temperature = ");
  Serial.print(mlx.readObjectTempF());
  Serial.println("�F");

  Serial.println("-----------------------------------------------------------------");
  temp_obj = mlx.readObjectTempC();

  TCA9548A(2);
  display.clearDisplay();
  display.setCursor(25, 10);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println(" Temperature");
  display.setCursor(25, 30);
  display.setTextSize(2);
  display.print(temp_obj);
  display.print((char)247);
  display.print("C");
  display.display();
  delay(1000);
}

//Prayer Times -----------------
void prayerTimes() {
  Serial.println("PTimes:");

  int dst = 1;

  set_calc_method(ISNA);
  set_asr_method(Shafii);
  set_high_lats_adjust_method(AngleBased);
  set_fajr_angle(20);
  set_isha_angle(18);

  GPS_prayerTimes();
  //MEKKA
  //float latitude=21.427378;
  //float longitude=39.814838;
  //get_prayer_times(year(), month(), day(), latitude, longitude, dst, times);
  get_prayer_times(Year, Month, Day, latitude2, longitude2, 8, times);

  Serial.print("YEAR:");
  Serial.println(year());
  Serial.print("MONTH:");
  Serial.println(month());
  Serial.print("DAY:");
  Serial.println(day());

  for (int i = 0; i < sizeof(times) / sizeof(double); i++) {
    char tmp[10];
    int hours, minutes;
    get_float_time_parts(times[i], hours, minutes);
    p("%d \t %10s %s \t %02d:%02d \n\r", i, TimeName[i], dtostrf(times[i], 2, 2, tmp), hours, minutes);
    /*
    Serial.print(i); 
    Serial.print(" \t "); 
    Serial.print(TimeName[i]); 
    Serial.print(" \t\t "); 
    Serial.print(times[i]);
    Serial.print(" \t ");     
    Serial.print(hours); 
    Serial.print(":"); 
    Serial.print(minutes); 
    Serial.println();
    */
  }
}

//GPS Scan-----------------
void GPS_prayerTimes() {
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 1000;) {
    while (neogps.available()) {
      //Location
      if (gps.encode(neogps.read())) {
        newData = true;
      }
      // get time from GPS module
      if (gps.time.isValid()) {
        Minute = gps.time.minute();
        Second = gps.time.second();
        Hour = gps.time.hour();
      }

      // get date drom GPS module
      if (gps.date.isValid()) {
        Day = gps.date.day();
        Month = gps.date.month();
        Year = gps.date.year();
      }
    }
  }

  //If newData is true
  if (newData == true) {
    newData = false;
    //Serial.println(gps.satellites.value());
    GPS_print_prayerTimes();
  } else {
    TCA9548A(2);
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(3);
    display.print("No Data");
    display.display();
  }
}

void GPS_print_prayerTimes() {
  TCA9548A(2);
  display.clearDisplay();
  display.setTextColor(WHITE);

  if (gps.location.isValid() == 0) {
    latitude2 = gps.location.lat();
    longitude2 = gps.location.lng();
    satellites2 = gps.satellites.value();
    altimeter2 = gps.altitude.meters();

  } else {
    TCA9548A(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print("GPS");

    display.setCursor(0, 20);
    display.setTextSize(1);
    display.print("Positioning");
    display.display();

    delay(100);
    display.clearDisplay();

    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print("GPS");

    display.setCursor(0, 20);
    display.setTextSize(1);
    display.print("Positioning.");
    display.display();

    delay(100);
    display.clearDisplay();

    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print("GPS");

    display.setCursor(0, 20);
    display.setTextSize(1);
    display.print("Positioning..");
    display.display();

    delay(100);
    display.clearDisplay();

    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print("GPS");

    display.setCursor(0, 20);
    display.setTextSize(1);
    display.print("Positioning...");
    display.display();
    delay(100);
  }
}
//-------------------------


void loop() {
  if (button == 1) {
    n = 0;
    timedate();
  } else if (button == 2) {
    GPS();
  } else if (button == 3) {
    speedometer();
  } else if (button == 4) {
    BME280_1();
  } else if (button == 5) {
    BME280_2();
  } else if (button == 6) {
    BH1750();
  } else if (button == 7) {
    //heartRate();
    MLX1();
  } else {
    //MLX2();
  }
  //timedate();
  //GPS();
}