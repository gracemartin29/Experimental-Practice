#include <ArduinoBLE.h>

BLEService motorService("bcd83f84-3f46-41df-b9f9-0d4e6365de5d"); // Bluetooth® Low Energy MOTOR Service

// Bluetooth® Low Energy MOTOR Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("bcd83f84-3f46-41df-b9f9-0d4e6365de5d", BLERead | BLEWrite);

const int motorPin = 13; // pin to use for the MOTOR

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // set MOTOR pin to output mode
  pinMode(motorPin, OUTPUT);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("MOTOR");
  BLE.setAdvertisedService(motorService);

  // add the characteristic to the service
  motorService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(motorService);

  // set the initial value for the characteristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE MOTOR Peripheral");
}

void loop() {
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());



    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the motor:
      if (switchCharacteristic.written()) {   // any value other than 0
        Serial.println("MOTOR on");
        analogWrite(motorPin, switchCharacteristic.value());         // will turn the motor on full power
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
