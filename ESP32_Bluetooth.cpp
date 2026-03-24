#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define LED_PIN 2

// UART-like BLE service
#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_RX   "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"  // phone writes here
#define CHARACTERISTIC_TX   "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  // ESP32 notifies here

BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) override {
    deviceConnected = true;
    Serial.println("Phone connected");
  }

  void onDisconnect(BLEServer *pServer) override {
    deviceConnected = false;
    Serial.println("Phone disconnected");
    BLEDevice::startAdvertising(); // restart advertising
  }
};

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    String value = pCharacteristic->getValue();

    if (value.length() > 0) {
      Serial.print("Received: ");
      Serial.println(value);

      value.trim();
      value.toUpperCase();

      if (value == "ON") {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED ON");
        if (deviceConnected) {
          pTxCharacteristic->setValue("LED is ON");
          pTxCharacteristic->notify();
        }
      } 
      else if (value == "OFF") {
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED OFF");
        if (deviceConnected) {
          pTxCharacteristic->setValue("LED is OFF");
          pTxCharacteristic->notify();
        }
      } 
      else {
        if (deviceConnected) {
          pTxCharacteristic->setValue("Unknown command. Use ON or OFF");
          pTxCharacteristic->notify();
        }
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  BLEDevice::init("ESP32_BLE_Prototype");

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pTxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_TX,
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_RX,
    BLECharacteristic::PROPERTY_WRITE
  );
  pRxCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("BLE prototype started. Waiting for phone connection...");
}

void loop() {
  delay(200);
}