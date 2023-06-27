#include "MQ7.h"

#define A_PIN 1
#define VOLTAGE 5

// init MQ7 device
MQ7 mq7(A_PIN, VOLTAGE);

float coppm;