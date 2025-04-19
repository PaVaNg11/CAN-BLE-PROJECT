#include <M5AtomS3.h>
#include <BLEDevice.h>
#include <stdio.h>

#define server_uuid "7bc7a676-b18c-435c-ac92-c50cf7f8091b"
#define char_uuid "dc25392b-c802-4994-883e-19a890d316cb"
#define read_uuid "73836fc4-0bdc-4fdf-a6d9-28b40a8031d3"
#define write_uuid "34fc0856-3b45-42fd-b7be-dd5e8f604876"
#define descriptor_uuid "e7199e9f-d73d-4d6a-be9f-86a3694cb21c"

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pChar;
BLECharacteristic *prChar;
BLECharacteristic *pwChar;
BLEAdvertising *pAdvertising;
BLEDescriptor *prDescriptor;
BLEDescriptor *pwDescriptor;

String val="";

void setup() {
  AtomS3.begin(true);
  Serial.begin(115200);
  BLEDevice::init("THIS IS ATOMS3 lite ");

  pServer = BLEDevice::createServer();
  pService = pServer->createService(server_uuid); 
  pChar=pService->createCharacteristic(char_uuid,BLECharacteristic::PROPERTY_NOTIFY);
  prChar=pService->createCharacteristic(read_uuid,BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  pwChar=pService->createCharacteristic(write_uuid,BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);

  prDescriptor= new BLEDescriptor("2901");  // use:- BLEDescriptor("2901") to have user define Decscriptor thereby we can have..
     /////////////////////////////////////// .. human readable texts and BLEDescriptor("2902") is for client characteristic configuration 
     ///////////////////////////////////////  can also contain the direct uuid BLEDescriptor(UUID) and send data 
     //////////////////////////////////////   be aware of how much of length of data we can send .
  pwDescriptor = new BLEDescriptor("2901");

    prChar->setValue("Hello Pavan");
    prDescriptor->setValue("READ_MESSG_FROM_THIS_CHAR");
    prChar->addDescriptor(prDescriptor);
    pwDescriptor->setValue("WRITE_MESSAGE_FROM_THIS_CHAR");
    pwChar->addDescriptor(pwDescriptor);

    pService->addCharacteristic(prChar);
    pService->addCharacteristic(pwChar);
    

  pService->start();
 
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(server_uuid);
  // pAdvertising->setScanResponse(true);
  // pAdvertising->setMinPreferred(0x06); 
  // pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("OK");
} 

void loop() {
  AtomS3.update();
  val=pwChar->getValue();
  if(val.length() > 0) {
   Serial.print(val);
   Serial.print(":lenght \t");
   Serial.println(pwChar->getLength());
   pwChar->setValue("");
  }
  if(Serial.available()){
    auto send_data=Serial.readString();
    Serial.println(send_data);
    prChar->setValue(send_data.c_str());
  }
  
  //Serial.println(pServer->getConnectedCount());
  delay(1000);
}
