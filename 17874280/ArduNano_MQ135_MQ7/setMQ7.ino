void set_MQ7(){
  Serial.println("Calibrating MQ7");
	mq7.calibrate();		// calculates R0
	Serial.println("Calibration done!");  
}

void read_MQ7(){
  Serial.print("CO Level: "); Serial.print(mq7.readPpm());
	Serial.println(" PPM"); 	// blank new line
  coppm = mq7.readPpm();
}