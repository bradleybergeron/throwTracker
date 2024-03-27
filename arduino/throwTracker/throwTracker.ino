/*
  LED

  This example creates a Bluetooth® Low Energy peripheral with service that contains a
  characteristic to control an LED.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.

  You can use a generic Bluetooth® Low Energy central app, like LightBlue (iOS and Android) or
  nRF Connect (Android), to interact with the services and characteristics
  created in this sketch.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>
#include <Arduino_BMI270_BMM150.h>

BLEService resetService("09dab2bd-59d9-462b-992a-45c52756a4ba"); // Bluetooth® Low Energy LED Service
BLEService gyroService("a77cb8c5-934a-45fb-9cff-55a7fc9ef739");
BLEService accelService("fd88e1a7-af90-4f15-a6c2-6964a9ddb95e");

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEBoolCharacteristic resetCharacteristic("41690ee6-2096-40b8-bf38-d0259d393596", BLERead | BLEWrite);
BLEFloatCharacteristic gyroXCharacteristic("520be596-d2f6-4ba4-b8df-a5b70ce5e097", BLERead);
BLEFloatCharacteristic gyroYCharacteristic("19da0275-4797-46e8-8190-e1a01b1502f4", BLERead);
BLEFloatCharacteristic gyroZCharacteristic("1e511e9b-14ff-4dfe-8eb4-91d24f26afc6", BLERead);
BLEFloatCharacteristic accelXCharacteristic("8bd3f963-1dce-4867-b746-51b2a74f05b2", BLERead);
BLEFloatCharacteristic accelYCharacteristic("6cf8fb90-fe25-4eae-a9ff-f9c2e0dd56fb", BLERead);
BLEFloatCharacteristic accelZCharacteristic("6cbe87a2-0956-4da8-9daf-081fcdfcaecf", BLERead);

const int ledPin = LED_BUILTIN; // pin to use for the LED

float gyroX,gyroY,gyroZ,gyroXMax,gyroYMax,gyroZMax;
float accelX,accelY,accelZ,accelXMax,accelYMax,accelZMax;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // set LED pin to output mode
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);         // when the central disconnects, turn off the LED
  digitalWrite(LEDR, HIGH);          // will turn the LED off
  digitalWrite(LEDG, HIGH);        // will turn the LED off
  digitalWrite(LEDB, HIGH);         // will turn the LED off

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
  
    // Set led to red - an error occurred
    digitalWrite(LEDR, LOW);          // will turn the LED off
    digitalWrite(LEDG, HIGH);        // will turn the LED off
    digitalWrite(LEDB, HIGH);         // will turn the LED off

    while (1);
  }
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);

    // Set led to red - an error occurred
    digitalWrite(LEDR, LOW);          // will turn the LED off
    digitalWrite(LEDG, HIGH);        // will turn the LED off
    digitalWrite(LEDB, HIGH);         // will turn the LED off
  }
  gyroXMax=0.0;
  gyroYMax=0.0;
  gyroZMax=0.0;
  accelXMax=0.0;
  accelYMax=0.0;
  accelZMax=0.0;

  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  Serial.println();

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");

  // set advertised local name and service UUID:
  BLE.setLocalName("Disc Golf Throw Tracker");
  BLE.setAdvertisedService(resetService);
  BLE.setAdvertisedService(gyroService);
  BLE.setAdvertisedService(accelService);

  // add the characteristic to the service
  resetService.addCharacteristic(resetCharacteristic);

  gyroService.addCharacteristic(gyroXCharacteristic);
  gyroService.addCharacteristic(gyroYCharacteristic);
  gyroService.addCharacteristic(gyroZCharacteristic);

  accelService.addCharacteristic(accelXCharacteristic);
  accelService.addCharacteristic(accelYCharacteristic);
  accelService.addCharacteristic(accelZCharacteristic);


  // add service
  BLE.addService(resetService);
  BLE.addService(gyroService);
  BLE.addService(accelService);

  // set the initial value for the characeristic:
  resetCharacteristic.writeValue(false);
  gyroXCharacteristic.writeValue(0);
  gyroYCharacteristic.writeValue(0);
  gyroZCharacteristic.writeValue(0);
  accelXCharacteristic.writeValue(0);
  accelYCharacteristic.writeValue(0);
  accelZCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("Disc Golf Throw Tracker Peripheral Ready...");
}

void loop() {

  // Set LED to blue - ready and waiting
  digitalWrite(LEDR, HIGH);          // will turn the LED off
  digitalWrite(LEDG, HIGH);        // will turn the LED off
  digitalWrite(LEDB, LOW);         // will turn the LED off

  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  // if a client is connected to the server:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    Serial.println("BLE Client Connected...");

    // while the central is still connected to peripheral:
    while (central.connected()) {

      // Set LED to Green - ready and client connected
      digitalWrite(LEDR, HIGH);          // will turn the LED off
      digitalWrite(LEDG, LOW);        // will turn the LED off
      digitalWrite(LEDB, HIGH);         // will turn the LED off

      // process gyroscope. Update Characteristic if a new max is found
      if (IMU.gyroscopeAvailable()) {
        IMU.readGyroscope(gyroX, gyroY, gyroZ);
        if (gyroX > gyroXMax) {
          gyroXMax = gyroX;
          gyroXCharacteristic.writeValue(gyroX);
        }
        if (gyroY > gyroYMax) {
          gyroYMax = gyroY;
          gyroYCharacteristic.writeValue(gyroY);
        }
        if (gyroZ > gyroZMax) {
          gyroZMax = gyroZ;
          gyroZCharacteristic.writeValue(gyroZ);
        }
      }

      // process acceleration. Update Characteristic if a new max is found
      if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(accelX, accelY, accelZ);
        if (accelX > accelXMax) {
          accelXMax = accelX;
          accelXCharacteristic.writeValue(accelX);
        }
        if (accelY > accelYMax) {
          accelYMax = accelY;
          accelYCharacteristic.writeValue(accelY);
        }
        if (accelZ > accelZMax) {
          accelZMax = accelZ;
          accelZCharacteristic.writeValue(accelZ);
        }
      }

      // check for reset from client
      if (resetCharacteristic.written()) {
        switch (resetCharacteristic.value()) {   // any value other than 0
          case true:
            gyroXMax=0;
            gyroYMax=0;
            gyroZMax=0;
            accelXMax=0;
            accelYMax=0;
            accelZMax=0;
            resetCharacteristic.writeValue(false);

            Serial.println("Max values were reset successfully.");
            break;
        }
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, LOW);         // when the central disconnects, turn off the LED
    digitalWrite(LEDR, HIGH);          // will turn the LED off
    digitalWrite(LEDG, HIGH);        // will turn the LED off
    digitalWrite(LEDB, HIGH);         // will turn the LED off
  }
}
