 /*!
  * @file  getGasPPM.ino
  * @brief Reading Gas concentration, A concentration of one part per million (PPM).
  * @n When using IIC device, select I2C address, set the dialing switch A0, A1 (Address_0 is [0 0]), (Address_1 is [1 0]), (Address_2 is [0 1]), (Address_3 is [1 1]).
  * @n When using the Breakout version, connect the adcPin and PowerPin
  * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  * @licence     The MIT License (MIT)
  * @author      ZhixinLiu(zhixin.liu@dfrobot.com)
  * @version     V1.1
  * @date        2021-04-19
  * @get         from https://www.dfrobot.com
  * @url         https://github.com/dfrobot/DFRobot_MICS
  */
#include "DFRobot_MICS.h"

#define CALIBRATION_TIME   3                  // Default calibration time is three minutes

// When using the Breakout version, use the following program to construct an object from DFRobot_MICS_ADC
/**!
  adcPin is A0~A5
  powerPin is General IO
*/
#define ADC_PIN   A0
#define POWER_PIN 10
DFRobot_MICS_ADC mics(/*adcPin*/ADC_PIN, /*powerPin*/POWER_PIN);

float nh3ppm;