#include <M5AtomS3.h>
#include <BLEDevice.h>
#include <BLE2902.h>    // not needed 
//#include <random.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <driver/twai.h>


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



String start_session="$apstrts#";
char x;
char hex_array[100];
int i=0,j=0,k=0,padding_length;
char hexChar2[100]="$dvrnd,";
char hexChar1[100]="";
char padded[9];
uint8_t hex_num[4],hex_encoded[4],temp;
uint8_t secret_code[4]={0xAB,0xCD,0xEF,0x12};
String data;
bool prevention_flag=false;

twai_message_t mssg;
twai_general_config_t g_config=TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_2,GPIO_NUM_1, TWAI_MODE_NORMAL);
twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
twai_message_t tx_mssg={
  .extd=0,
  .rtr=0,
  .ss=0,
  .self=0,
  .dlc_non_comp=0,
  .identifier=0xC,
  .data_length_code=4,
  .data={0x1,0x2,0x3,0x4},
};


void setup() {

Serial.begin(115200);
AtomS3.begin(true);
esp_bd_addr_t newAddress = {0x24, 0x6F, 0x28, 0xA1, 0xB2, 0xC3};  // for manually setting the MAC address use pAdv->setDeviceAddress(newAddress)

BLEDevice::init("ATOM_S3_LITE");

pServer_1=BLEDevice::createServer();
                                                   

pService_1=pServer_1->createService(service_1_uuid);
pService_2=pServer_1->createService(service_2_uuid);

//Service 1 Characteristics
pChar_1_N=pService_1->createCharacteristic(char_1_N_uuid,BLECharacteristic::PROPERTY_NOTIFY);
pChar_1_R=pService_1->createCharacteristic(char_1_R_uuid,BLECharacteristic::PROPERTY_READ);

// Service 2 Characteristics 
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
pAdv_1->setDeviceAddress(newAddress);             // sets the manual MAC address for the device.
BLEDevice::startAdvertising();

if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        Serial.println("Driver installed");
    } else {
        Serial.println("Failed to install driver");   
    }
if (twai_start() == ESP_OK) {
        Serial.println("Driver started");
    } else {
        Serial.println("Failed to start driver");

    }



delay(10);

Serial.println("START");


}

// void write_to_busmaster(){
//   tx_mssg.data=pChar_2_W->getData();
//   if(twai_transmit(&tx_mssg,pdMS_TO_TICKS(10000))==ESP_OK){
//     Serial.println("Sent..");
//   }}


void send_can_data(){
  Serial.println("Message received");
  Serial.printf("ID is %X\n", mssg.identifier);
  Serial.print("DATA:");
  if (!(mssg.rtr)) {
    for (int i = 0; i < mssg.data_length_code; i++) {
        Serial.printf("%02X",mssg.data[i]);
    }
  pChar_1_N->setValue(mssg.data,sizeof(mssg.data));
  pChar_1_N->notify();
  delay(500);
}
Serial.println();
}



void hex_fn(unsigned int num ){
  if(num==0){
    num=random(0,pow(2,32));                 
  } 
  // Serial.printf("size of hex_num:%d\n",sizeof(hex_num));
      for(i=0;i<sizeof(hex_num);i++){
        hex_num[i]=num & 0xFF;
        num=num>>8;
      }
 //Serial.println(i);
      
      for (j = 0; j < sizeof(hex_num)/2; j++) {
        temp = hex_num[j];
        hex_num[j] = hex_num[i - j - 1];
        hex_num[i - j - 1] = temp;
    }
    for(k=0;k<sizeof(hex_encoded);k++){
        hex_encoded[k]=hex_num[k]^secret_code[k];
      }
      Serial.printf("HEX_ENCODED_WITH_XOR:");
      for(k=0;k<sizeof(hex_encoded);k++){
        Serial.printf("%02X ",hex_encoded[k]);
      }
Serial.println();
} 


const char*  rn_fn(unsigned int  num ,int rem){ 
     
  while(num != 0){
        rem=num%16;
        num=num/16;
        
       if (rem >= 10) {
          hex_array[i] = 'A' + (rem - 10);  
        } else {
          hex_array[i] = '0' + rem;  
        }
        i++;     
  }   
  int length=i; 
   if(length>=8){
      
       for(j=0;j<length;j++){
           strncat(hexChar2,&hex_array[length-j-1],1);               // logic to append zeros 
       }
       return strcat(hexChar2,"#");
   }
   else{
      for(j=0;j<length;j++){
           strncat(hexChar1,&hex_array[length-j-1],1);
       }
       padding_length=8-length;
       memset(padded,'0',padding_length);
       padded[padding_length]='\0';
       strcat(padded,hexChar1);
       strcat(hexChar2,padded);
       strcat(hexChar2,"#");
       return hexChar2; 
}
}

void loop() {
    
  auto val = pChar_2_W->getValue();
  if(val.length()>0){
    if( val==start_session){
      Serial.printf("%s-Command received \n",val.c_str());
      unsigned int num=random(0,pow(2,32));
      hex_fn(num);
      data=rn_fn(num,0); 
      Serial.println(num);
      Serial.println(data);
      pChar_1_N->setValue(data);
      pChar_1_N->notify();
    
      // Serial.printf("UUID:%s \n",pChar_2_W->getUUID().toString().c_str());     /// returns the UUID for the characteristic
      // Serial.printf("MAC Address:%s \n",BLEDevice::getAddress().toString().c_str());  /// returns the actual Device MAC address
      prevention_flag=true; 
      i=0;
      padding_length=0;
      data="";
      strcpy(hex_array,"");
      strcpy(hexChar1,"");   // Reinitialize zeroes and null to variable for next command/next random number generation 
      strcpy(hexChar2,"");
      strcpy(hexChar2,"$dvrnd,");
      pChar_2_W->setValue("");
      pChar_1_N->setValue("");
      //Serial.println("executed all the initialization");
     
  }
  else if((val[0]==0x27) && (prevention_flag)){
    bool flag=false;
    Serial.print("Recieved the key:");
          for(i=0;i<5;i++){
          Serial.printf("%02X ",val[i]);
          }
    Serial.println();
    //Serial.printf("%d\n",val.length());
    for(i=1;i<5;i++){
        if(val[i] != hex_encoded[i-1] ){
          Serial.printf("%02X:%02X\n",val[i],hex_encoded[i-1]);
          flag=false;
          break;
                                     }
    
        else{
        Serial.printf("%02X:%02X\n",val[i],hex_encoded[i-1]);
        flag=true;
            }
    }
    if(flag){
      Serial.println("KEYS MATCH");
      pChar_1_N->setValue("keysmatch_readyfor_transmission");
      pChar_1_N->notify();
      uint8_t read_hex_array[]={0x67,0x02};
      pChar_1_R->setValue(read_hex_array,sizeof(read_hex_array));
      pChar_1_R->notify();
      while(true){
     if(twai_receive(&mssg,pdMS_TO_TICKS(20000))==ESP_OK){
                 send_can_data();
                                }
                                else{
                                  Serial.println("No data incoming from CAN breaking out of the loop 20 seconds over  ");
                                  break;
                                }
      }
     
            }
    else{
      Serial.println("KEYS do not match");
      pChar_1_N->setValue("Security_Access_Denied");
      pChar_1_N->notify();
        }
    pChar_2_W->setValue("");
  }

      
  else{
    Serial.println("Wrong_command");
    pChar_1_N->setValue("Wrong command");
    pChar_1_N->notify();
    pChar_2_W->setValue("");

  }
  }

    // if(Serial.available()){
    //   auto data =Serial.readString();
    //   Serial.println(data);
    //   pChar_1_R->setValue(data);
    //   pChar_1_R->notify();
    // }
        
  delay(500);
}




