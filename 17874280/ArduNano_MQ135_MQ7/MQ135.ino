void read_MQ135() {

  co2raw = 0;   //int for raw value of co2
  co2comp = 0;  //int for compensated co2
  co2ppm = 0;   //int for calculated ppm
  zzz = 0;      //int for averaging
  grafX = 0;
  
  for (int x = 0; x < 10; x++) {  //samplpe co2 10x over 2 seconds
    co2now[x] = analogRead(A2);
    delay(200);
  }

  for (int x = 0; x < 10; x++) {  //add samples together
    zzz = zzz + co2now[x];
  }
  co2raw = zzz / 10;                          //divide samples by 10
  co2comp = co2raw - co2Zero;                 //get compensated value
  co2ppm = map(co2comp, 0, 1023, 400, 5000);  //map value for atmospheric levels
                                              //set cursor
  Serial.print("CO2 Level: ");                //skip a line
  Serial.print(co2ppm);                       //print co2 ppm
  Serial.println(" PPM");                     //print units                       //show the buffer
  if (co2ppm > 999) {                         //if co2 ppm > 1000
    digitalWrite(led, HIGH);                  //turn on led
  } else {                                    //if not
    digitalWrite(led, LOW);                   //turn off led
  }
}