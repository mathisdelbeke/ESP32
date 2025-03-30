#include <Arduino.h>
#include <NimBLEDevice.h>  // Use NimBLE instead of BLEDevice

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

NimBLEServer* pServer = nullptr;
NimBLECharacteristic* pCharacteristic = nullptr;

const int potmeterPin = 34;
int lastValue = 0;

// Custom callback class for handling writes
class MyCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
        std::string value = pCharacteristic->getValue();
        if (!value.empty()) {
            Serial.print("Received Value: ");
            Serial.println(value.c_str());  
        }
    }
};

void setup() {
    Serial.begin(115200);
    
    NimBLEDevice::init("ESP32_BLE"); 
    pServer = NimBLEDevice::createServer();

    NimBLEService* pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        NIMBLE_PROPERTY::READ |
                        NIMBLE_PROPERTY::WRITE |
                        NIMBLE_PROPERTY::NOTIFY
                      );
    
    // Attach the callback to handle write events
    pCharacteristic->setCallbacks(new MyCallbacks());

    pService->start();

    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->start();

    Serial.println("BLE is ready!");
}

void loop() {
    lastValue = analogRead(potmeterPin);
    Serial.println(lastValue);
    pCharacteristic->setValue(lastValue);
    pCharacteristic->notify();
    delay(1000);
}
