/*
  Arduino BMI270 - Simple Gyroscope

  This example reads the gyroscope values from the BMI270
  sensor and continuously prints them to the Serial Monitor
  or Serial Plotter.

  The circuit:
  - Arduino Nano 33 BLE Sense Rev2

  created 10 Jul 2019
  by Riccardo Rizzo

  This example code is in the public domain.
*/

#include "Arduino_BMI270_BMM150.h"

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  Serial.println();
}

void loop() {
  float x, y, z;
  int plusThreshold = 30;
  int minusThreshold = -30;

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(x, y, z);

    if(y > plusThreshold)
    {
    Serial.println("Collision front");
    delay(500);
    }
    if(y < minusThreshold)
    {
    Serial.println("Collision back");
    delay(500);
    }
    if(x < minusThreshold)
    {
    Serial.println("Collision right");
    delay(500);
    }
    if(x > plusThreshold)
    {
    Serial.println("Collision left");
    delay(500);
    }
  }
}
