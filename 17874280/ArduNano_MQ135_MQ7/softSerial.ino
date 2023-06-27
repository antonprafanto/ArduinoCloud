void sendData_toESP32(){
  Serial.println("_______________________");
  str =String('*')+String(co2ppm)+String(',')+String(coppm)+String(',')+String(nh3ppm)+String('#');
  mySerial.println(str);
}