#include <M5AtomS3.h>
#include <BLEDevice.h>
#include <BLE2902.h>    // not needed 


#define service_1_uuid "7bc7a676-b18c-435c-ac92-c50cf7f8091b"       // service UUID's for two Services
#define service_2_uuid "dc25392b-c802-4994-883e-19a890d316cb"

#define char_1_N_uuid "73836fc4-0bdc-4fdf-a6d9-28b40a8031d3"        // Characteristics UUID's in two Services
#define char_1_R_uuid "34fc0856-3b45-42fd-b7be-dd5e8f604876"
#define char_2_W_uuid "e7199e9f-d73d-4d6a-be9f-86a3694cb21c"


BLEServer *pServer_1;
BLEServer *pServer_2;

BLEService *pService_1;
BLEService *pService_2;

BLECharacteristic *pChar_1_N;
BLECharacteristic *pChar_1_R;                                
BLECharacteristic *pChar_2_W;

BLEAdvertising *pAdv_1;
BLEAdvertising *pAdv_2;

BLEDescriptor *pDes;

BLE2902 *p2902_1;
BLE2902 *p2902_2;

int count=0;

void setup() {

Serial.begin(115200);
AtomS3.begin(true);
esp_bd_addr_t newAddress = {0x24, 0x6F, 0x28, 0xA1, 0xB2, 0xC3};  // for manually setting the MAC address use pAdv->setDeviceAddress(newAddress)

BLEDevice::init("ATOM_S3_LITE");

pServer_1=BLEDevice::createServer();
                                                   //pServer_2=BLEDevice::createServer();

pService_1=pServer_1->createService(service_1_uuid);
pService_2=pServer_1->createService(service_2_uuid);

//Server 1 Characteristics
pChar_1_N=pService_1->createCharacteristic(char_1_N_uuid,BLECharacteristic::PROPERTY_NOTIFY);
pChar_1_R=pService_1->createCharacteristic(char_1_R_uuid,BLECharacteristic::PROPERTY_READ);

// Server 2 Characteristics 
pChar_2_W=pService_2->createCharacteristic(char_2_W_uuid,BLECharacteristic::PROPERTY_WRITE);


pService_1->addCharacteristic(pChar_1_N);
pService_1->addCharacteristic(pChar_1_R);
//pService_2->addCharacteristic(pChar_2_W);


//pDes=new BLEDescriptor("2902");
p2902_1= new BLE2902();
p2902_1->setNotifications(true);         // Descriptor for Notify characteristics to automatically enable notifications.
pChar_1_N->addDescriptor(p2902_1);


p2902_2 = new BLE2902();                //Descriptor for READ Characteristics
p2902_2->setNotifications(true);   
pChar_1_R->addDescriptor(p2902_2);

pChar_2_W->setWriteProperty(true);    // enables the client to WRITE into characteristics 
pChar_1_R->setReadProperty(true);     // enables the client to READ from the server 

pService_1->start();
pService_2->start();

pAdv_1=BLEDevice::getAdvertising();
pAdv_1->addServiceUUID(service_1_uuid);
pAdv_1->addServiceUUID(service_2_uuid);
pAdv_1->setAppearance(0x05C0);                     // changes the icon in the nRF application 
pAdv_1->setDeviceAddress(newAddress);              // sets the manual MAC address for the device.
BLEDevice::startAdvertising();

delay(10);
Serial.println("START");


}

void loop() {
    //count++;
    auto val = pChar_2_W->getValue();
  if(val.length()>0 ){
      Serial.printf("%s:length:%d\n",val.c_str(),val.length());
      pChar_1_N->setValue(val);
      pChar_1_N->notify();
    
      Serial.printf("UUID:%s \n",pChar_2_W->getUUID().toString().c_str());     /// returns the UUID for the characteristic
      Serial.printf("MAC Address:%s \n",BLEDevice::getAddress().toString().c_str());  /// returns the actual Device MAC address 

      pChar_2_W->setValue("");
      pChar_1_N->setValue("");
      
  }
    if(Serial.available()){
      auto data =Serial.readString();
      Serial.println(data);
      pChar_1_R->setValue(data);
      pChar_1_R->notify();
    }

    
  delay(500);
}




