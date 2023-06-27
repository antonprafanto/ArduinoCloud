#include "MICS5524.h"
#include "setMQ7.h"
#include "MQ135.h"
#include "softSerial.h"

void setup() {
  pinMode(anInput,INPUT);                     //MQ135 analog feed set for input
  pinMode(digTrigger,INPUT);                  //MQ135 digital feed set for input
  pinMode(led,OUTPUT);                        //led set for output
  Serial.begin(9600);  
  mySerial.begin(9600);  

  setup_MICS5524();
  set_MQ7();                //serial comms for debuging
}

void loop() {
  read_MQ135();
  read_MQ7();
  read_MICS5524();
  sendData_toESP32();
}