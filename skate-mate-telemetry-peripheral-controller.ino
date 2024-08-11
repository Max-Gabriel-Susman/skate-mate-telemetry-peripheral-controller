#include <ArduinoBLE.h>

// Define the BLE service and characteristic UUIDs
BLEService customService("E20A39F4-73F5-4BC4-A12F-17D1AD07A961");  // Custom service UUID
BLECharacteristic customCharacteristic("2A56", BLERead | BLEWrite | BLENotify, 20); // Modified to include Notify

// Variable to store the characteristic value
unsigned char charValue = 0;
const String dataToSend = "Hello from Arduino"; // The hardcoded string to send
int sendDataIndex = 0; // Index for the data to send

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
      // Send data
      sendData();

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
    sendDataIndex = 0; // Reset the index when disconnected
  }
}

void sendData() {
  if (sendDataIndex >= dataToSend.length()) {
    return; // No data left to send
  }

  // Calculate the amount of data to send
  int amountToSend = min(20, dataToSend.length() - sendDataIndex); // BLE can send a maximum of 20 bytes at a time
  String chunk = dataToSend.substring(sendDataIndex, sendDataIndex + amountToSend);
  char buffer[20];
  chunk.toCharArray(buffer, amountToSend + 1);

  // Send the data
  bool didSend = customCharacteristic.writeValue((const void*)buffer, amountToSend);
  if (didSend) {
    Serial.print("Sent: ");
    Serial.println(chunk);
    sendDataIndex += amountToSend;
  } else {
    Serial.println("Failed to send data");
  }
}
