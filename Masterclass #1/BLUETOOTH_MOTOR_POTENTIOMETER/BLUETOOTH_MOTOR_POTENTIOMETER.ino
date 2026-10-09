/*
  MOTOR Control

  This example scans for Bluetooth® Low Energy peripherals until one with the advertised service
  "19b10000-e8f2-537e-4f6c-d104768a1214" UUID is found. Once discovered and connected,
  it will remotely control the Bluetooth® Low Energy peripheral's MOTOR, when the button is pressed or released.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.
  - Button with pull-up resistor connected to pin 2.

  You can use it with another board that is compatible with this library and the
  Peripherals -> MOTOR example.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>

// variables for button
int potPin = A3;
int potVal = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);


  // initialize the Bluetooth® Low Energy hardware
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }

  Serial.println("Bluetooth® Low Energy Central - MOTOR control");

  // start scanning for peripherals
  BLE.scanForUuid("bcd83f84-3f46-41df-b9f9-0d4e6365de5d");
}

void loop() {
  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    // discovered a peripheral, print out address, local name, and advertised service
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();

    if (peripheral.localName() != "MOTOR") {
      return;
    }

    // stop scanning
    BLE.stopScan();

    controlMotor(peripheral);

    // peripheral disconnected, start scanning again
    BLE.scanForUuid("bcd83f84-3f46-41df-b9f9-0d4e6365de5d");
  }
}

void controlMotor(BLEDevice peripheral) {
  // connect to the peripheral
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  // retrieve the MOTOR characteristic
  BLECharacteristic motorCharacteristic = peripheral.characteristic("bcd83f84-3f46-41df-b9f9-0d4e6365de5d");

  if (!motorCharacteristic) {
    Serial.println("Peripheral does not have MOTOR characteristic!");
    peripheral.disconnect();
    return;
  } else if (!motorCharacteristic.canWrite()) {
    Serial.println("Peripheral does not have a writable MOTOR characteristic!");
    peripheral.disconnect();
    return;
  }

  while (peripheral.connected()) {
    // while the peripheral is connected

    // read the button pin
    potVal = analogRead(potPin);

    // Serial.println(potVal);

    motorCharacteristic.writeValue((byte)potVal);
    Serial.println((byte)potVal);
    delay(100);


  }

  Serial.println("Peripheral disconnected");
}
