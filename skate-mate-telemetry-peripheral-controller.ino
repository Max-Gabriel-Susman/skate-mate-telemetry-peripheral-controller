#include <ArduinoBLE.h>

// Define the BLE service and characteristic UUIDs
BLEService customService("180C");  // Custom service UUID
BLEUnsignedCharCharacteristic customCharacteristic("2A56", BLERead | BLEWrite);

// Variable to store the characteristic value
unsigned char charValue = 0;

void setup() {
  setupBLE();
}

void setupBLE() {
  Serial.begin(9600);
  while (!Serial);

  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("Failed to initialize BLE!");
    while (1);
  }

  // Set the local name and add the service
  BLE.setLocalName("SkateMateLeftIMUPeripheral");
  BLE.setAdvertisedService(customService);

  // Add the characteristic to the service
  customService.addCharacteristic(customCharacteristic);

  // Add the service to the BLE stack
  BLE.addService(customService);

  // Set the initial value of the characteristic
  customCharacteristic.writeValue(charValue);

  // Start advertising
  BLE.advertise();

  Serial.println("BLE Peripheral Device");
  Serial.println("Advertising...");
}

void loop() {
  loopBLE();
}

void loopBLE() {
  // Listen for BLE central connections
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    // Keep connected to the central
    while (central.connected()) {
      // If the characteristic value has been changed by the central
      if (customCharacteristic.written()) {
        charValue = customCharacteristic.value();
        Serial.print("Characteristic value updated to: ");
        Serial.println(charValue);

        // Respond to the change if needed
        // (e.g., turn on an LED, start a motor, etc.)
      }
    }

    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
