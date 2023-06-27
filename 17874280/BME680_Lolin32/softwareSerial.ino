void setupSerial() {
  dataIn = "";
  //Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

void recParsingData() {
  if (Serial2.available() > 0) {
    char inChar = (char)Serial2.read();
    dataIn += inChar;
    if (inChar == '\n') {
      parsing = true;
    }
  }

  if (parsing) {
    parsingData();
    parsing = false;
    dataIn = "";
  }
}

void parsingData() {
  int j = 0;

  //kirim data yang telah diterima sebelumnya
  /*Serial.print("data masuk : ");
  Serial.print(dataIn);
  Serial.print("\n");*/

  //inisialisasi variabel, (reset isi variabel)
  dt[j] = "";
  //proses parsing data
  for (i = 1; i < dataIn.length(); i++) {
    //pengecekan tiap karakter dengan karakter (#) dan (,)
    if ((dataIn[i] == '#') || (dataIn[i] == ',')) {
      //increment variabel j, digunakan untuk merubah index array penampung
      j++;
      dt[j] = "";  //inisialisasi variabel array dt[j]
    } else {
      //proses tampung data saat pengecekan karakter selesai.
      dt[j] = dt[j] + dataIn[i];
    }
  }

  //kirim data hasil parsing
  /*Serial.print("data 1 : ");
  Serial.print(dt[0].toFloat());
  Serial.print("\n");
  Serial.print("data 2 : ");
  Serial.print(dt[1].toFloat());
  Serial.print("\n");
  Serial.print("data 3 : ");
  Serial.print(dt[2].toFloat());
  Serial.print("\n\n");*/

  co2ppm = dt[0].toFloat();
  coppm = dt[1].toFloat();
  nh3ppm = dt[2].toFloat();

  Serial.print("CO2 Level: ");  //skip a line
  Serial.print(co2ppm);         //print co2 ppm
  Serial.println(" PPM");

  Serial.print("CO Level: ");
  Serial.print(coppm);
  Serial.println(" PPM");  // blank new line

  Serial.print("NH3 Level: ");
  Serial.print(nh3ppm);
  Serial.println(" PPM");  // blank new line

  Serial.println();
  Serial.println();
}