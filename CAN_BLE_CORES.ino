#include <M5AtomS3.h>
#include <BLEDevice.h>
#include <BLE2902.h>    
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

BLEScan *pBLEScan;
BLEScanResults *pBLERes;

String start_session="$apstrts#";
char x;
char hex_array[100];
int i=0,j=0,k=0,padding_length;
char hexChar2[100]="$dvrnd,";
char hexChar1[100]="";
char padded[9];
uint8_t hex_num[4],hex_encoded[4],temp,actual_data[4];
uint8_t secret_code[4]={0xAB,0xCD,0xEF,0x12};
String data;
bool prevention_flag=false,start_can=false;


twai_message_t mssg;



typedef struct{
  char *name;
  uint8_t pid[8];

}OBD2_struct;

OBD2_struct OBD2_commands[]={
  { "Supported PIDs in the range 01 - 20", { 0x01, 0x00 } },
    { "Monitor status since DTCs cleared", { 0x01, 0x01 } },
    { "Freeze DTC", { 0x01, 0x02 } },
    { "Fuel system status", { 0x01, 0x03 } },
    { "Calculated engine load", { 0x01, 0x04 } },
    { "Engine coolant temperature", { 0x01, 0x05 } },
    { "Short term fuel trim—Bank 1", { 0x01, 0x06 } },
    { "Long term fuel trim—Bank 1", { 0x01, 0x07 } },
    { "Short term fuel trim—Bank 2", { 0x01, 0x08 } },
    { "Long term fuel trim—Bank 2", { 0x01, 0x09 } },
    { "Fuel pressure (gauge pressure)", { 0x01, 0x0A } },
    { "Intake manifold absolute pressure", { 0x01, 0x0B } },
    { "Engine RPM", { 0x01, 0x0C } },
    { "Vehicle speed", { 0x01, 0x0D } },
    { "Timing advance", { 0x01, 0x0E } },
    { "Intake air temperature", { 0x01, 0x0F } },
    { "MAF air flow rate", { 0x01, 0x10 } },
    { "Throttle position", { 0x01, 0x11 } },
    { "Commanded secondary air status", { 0x01, 0x12 } },
    { "Oxygen sensors present", { 0x01, 0x13 } },
    { "Oxygen sensor 1", { 0x01, 0x14 } },
    { "Oxygen sensor 2", { 0x01, 0x15 } },
    { "Oxygen sensor 3", { 0x01, 0x16 } },
    { "Oxygen sensor 4", { 0x01, 0x17 } },
    { "Oxygen sensor 5", { 0x01, 0x18 } },
    { "Oxygen sensor 6", { 0x01, 0x19 } },
    { "Oxygen sensor 7", { 0x01, 0x1A } },
    { "Oxygen sensor 8", { 0x01, 0x1B } },
    { "OBD standards this vehicle conforms to", { 0x01, 0x1C } },
    { "Oxygen sensors present in 4 banks", { 0x01, 0x1D } },
    { "Auxiliary input status", { 0x01, 0x1E } },
    { "Run time since engine start", { 0x01, 0x1F } },
    { "Supported PIDs in the range 21 - 40", { 0x01, 0x20 } },
    { "Distance traveled with malfunction indicator lamp on", { 0x01, 0x21 } },
    { "Fuel rail pressure (relative to mainfold vacuum", { 0x01, 0x22 } },
    { "Fuel rail gauge pressure (diesel, or gasoline direct injection", { 0x01, 0x23 } },
    { "Oxygen sensor 1", { 0x01, 0x24 } },
    { "Oxygen sensor 2", { 0x01, 0x25 } },
    { "Oxygen sensor 3", { 0x01, 0x26 } },
    { "Oxygen sensor 4", { 0x01, 0x27 } },
    { "Oxygen sensor 5", { 0x01, 0x28 } },
    { "Oxygen sensor 6", { 0x01, 0x29 } },
    { "Oxygen sensor 7", { 0x01, 0x2A } },
    { "Oxygen sensor 8", { 0x01, 0x2B } },
    { "Commanded EGR", { 0x01, 0x2C } },
    { "EGR error", { 0x01, 0x2D } },
    { "Commanded evaporative purge", { 0x01, 0x2E } },
    { "Fuel tank level input", { 0x01, 0x2F } },
    { "Warm-ups since codes cleared", { 0x01, 0x30 } },
    { "Distance traveled since codes cleared", { 0x01, 0x31 } },
    { "Evaporative system vapor pressure", { 0x01, 0x32 } },
    { "Absolute barometric pressure", { 0x01, 0x33 } },
    { "Oxygen sensor 1", { 0x01, 0x34 } },
    { "Oxygen sensor 2", { 0x01, 0x35 } },
    { "Oxygen sensor 3", { 0x01, 0x36 } },
    { "Oxygen sensor 4", { 0x01, 0x37 } },
    { "Oxygen sensor 5", { 0x01, 0x38 } },
    { "Oxygen sensor 6", { 0x01, 0x39 } },
    { "Oxygen sensor 7", { 0x01, 0x3A } },
    { "Oxygen sensor 8", { 0x01, 0x3B } },
    { "Catalyst temperature, bank 1, sensor 1", { 0x01, 0x3C } },
    { "Catalyst temperature, bank 2, sensor 1", { 0x01, 0x3D } },
    { "Catalyst temperature, bank 1, sensor 2", { 0x01, 0x3E } },
    { "Catalyst temperature, bank 2, sensor 2", { 0x01, 0x3F } },
    { "Supported PIDs in the range 41 - 60", { 0x01, 0x40 } },
    { "Monitor status this drive cycle", { 0x01, 0x41 } },
    { "Control module voltage", { 0x01, 0x42 } },
    { "Absolute load value", { 0x01, 0x43 } },
    { "Fuel-Air commanded equivalence ratio", { 0x01, 0x44 } },
    { "Relative throttle position", { 0x01, 0x45 } },
    { "Ambient air temperature", { 0x01, 0x46 } },
    { "Absolute throttle position B", { 0x01, 0x47 } },
    { "Absolute throttle position C", { 0x01, 0x48 } },
    { "Accelerator pedal position D", { 0x01, 0x49 } },
    { "Accelerator pedal position E", { 0x01, 0x4A } },
    { "Accelerator pedal position F", { 0x01, 0x4B } },
    { "Commanded throttle actuator", { 0x01, 0x4C } },
    { "Time run with MIL on", { 0x01, 0x4D } },
    { "Time since trouble codes cleared", { 0x01, 0x4E } }
};

twai_message_t engine_speed_req={
  .extd=0,
  .rtr=0,
  .ss=0,
  .self=0,
  .dlc_non_comp=0,
  .identifier=0x7DF,
  .data_length_code=3,
  .data={0x02,0x01,0x0C},
};
twai_message_t vehical_speed_req={
  .extd=0,
  .rtr=0,
  .ss=0,
  .self=0,
  .dlc_non_comp=0,
  .identifier=0x7DF,
  .data_length_code=3,
  .data={0x02,0x01,0x0D},
};
twai_message_t engine_coolant_temp={
  .extd=0,
  .rtr=0,
  .ss=0,
  .self=0,
  .dlc_non_comp=0,
  .identifier=0x7DF,
  .data_length_code=3,
  .data={0x02,0x01,0x05},
};
twai_message_t send_multi_req={
  .extd=0,
  .rtr=0,
  .ss=0,
  .self=0,
  .dlc_non_comp=0,
  .identifier=0x7DF,
  .data_length_code=3,
  .data={0x02,0x01,0x00},
};

QueueHandle_t data_q;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
  p2902_1->setNotifications(true);         // Descriptor for Notify characteristics to automatically enable notifications.Otherwise i have to enable in the app manually.
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
  // pBLEScan=BLEDevice::getScan();
  // pBLEScan->setActiveScan(true);

  Serial.println("BLE setup done");

    twai_general_config_t g_config=TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_2,GPIO_NUM_1, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

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
data_q = xQueueCreate(5, sizeof(int));
xTaskCreatePinnedToCore(CAN_FN,"CAN_FN",4096,NULL,1,NULL,0);
xTaskCreatePinnedToCore(BLE_FN,"BLE_FN",4096,NULL,1,NULL,1);
}

int decode_OBD_data(uint8_t res_pid, uint8_t *data) {
  switch (res_pid) {
    case 0x04: { // Calculated engine load
      int load = (data[0] * 100) / 255;
      Serial.printf("Engine Load: %d%%\n", load);
      return load;
    }

    case 0x05: { // Engine coolant temperature
      int temp = data[0] - 40;
      Serial.printf("Coolant Temp: %d°C\n", temp);
      return temp;
    }

    case 0x0C: { // Engine RPM
      int rpm = ((data[0] << 8) | data[1]) / 4;
      Serial.printf("RPM: %d\n", rpm);
      return rpm;
    }

    case 0x0D: { // Vehicle speed
      int speed = data[0];
      Serial.printf("Vehicle Speed: %d km/h\n", speed);
      return speed;
    }

    case 0x0F: { // Intake air temperature
      int intakeTemp = data[0] - 40;
      Serial.printf("Intake Air Temp: %d°C\n", intakeTemp);
      return intakeTemp;
    }

    case 0x10: { // MAF air flow rate
      int maf = ((data[0] << 8) | data[1]) / 100;
      Serial.printf("MAF Flow Rate: %d g/s\n", maf);
      return maf;
    }

    case 0x11: { // Throttle position
      int throttle = (data[0] * 100) / 255;
      Serial.printf("Throttle Position: %d%%\n", throttle);
      return throttle;
    }

    case 0x0E: { // Timing advance
      int advance = (data[0] / 2) - 64;
      Serial.printf("Timing Advance: %d°\n", advance);
      return advance;
    }

    case 0x2F: { // Fuel tank level input
      int fuelLevel = (data[0] * 100) / 255;
      Serial.printf("Fuel Level: %d%%\n", fuelLevel);
      return fuelLevel;
    }

    case 0x1F: { // Run time since engine start
      int runTime = (data[0] << 8) | data[1];
      Serial.printf("Run Time: %d seconds\n", runTime);
      return runTime;
    }

    case 0x31: { // Distance traveled since codes cleared
      int distance = (data[0] << 8) | data[1];
      Serial.printf("Distance Since Codes Cleared: %d km\n", distance);
      return distance;
    }

    default:
      Serial.printf("Unsupported PID: 0x%02X\n", res_pid);
      return -1;
  }
}
const char*  rn_fn(unsigned int  num ,int rem){
   snprintf(hexChar1,sizeof(hexChar1),"$dvrnd,%08X#",num);
      return hexChar1;   
  // while(num != 0){
  //       rem=num%16;
  //       num=num/16;
        
  //      if (rem >= 10) {
  //         hex_array[i] = 'A' + (rem - 10);  
  //       } else {
  //         hex_array[i] = '0' + rem;  
  //       }
  //       i++;     
  // }   
  // int length=i; 
  //  if(length>=8){
      
  //      for(j=0;j<length;j++){
  //          strncat(hexChar2,&hex_array[length-j-1],1);               // logic to append zeros 
  //      }
  //      return strcat(hexChar2,"#");
  //  }
  //  else{
  //     for(j=0;j<length;j++){
  //          strncat(hexChar1,&hex_array[length-j-1],1);
  //      }
  //      padding_length=8-length;
  //      memset(padded,'0',padding_length);
  //      padded[padding_length]='\0';
  //      strcat(padded,hexChar1);
  //      strcat(hexChar2,padded);
  //      strcat(hexChar2,"#");
  //      return hexChar2; 
  //     }
     
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

void CAN_FN(void *pvParameters){
  while(1){
  if(start_can){
        if(twai_transmit(&engine_speed_req,pdMS_TO_TICKS(100))==ESP_OK){
          Serial.println("Engine Speed Request Sent..");
        }
          vTaskDelay(pdMS_TO_TICKS(10));
        if(twai_receive(&mssg,pdMS_TO_TICKS(150))==ESP_OK){
            Serial.printf("ID is %X\n", mssg.identifier);
            Serial.print("DATA:");

          if (!(mssg.rtr)) {
                  for (int i = 0; i < mssg.data_length_code; i++) {
                      Serial.printf("%02X",mssg.data[i]);
                  }
                  Serial.println();
                  if((0x7E8 <= mssg.identifier)&&(mssg.identifier <= 0x7EF) && ((0x41 <=mssg.data[1])&&(mssg.data[1] <= 0x4A))){

                    Serial.println("OK identifier");
                    uint8_t length_of_raw_data=mssg.data[0]-2;

                    if(length_of_raw_data>4){
                      length_of_raw_data=4;
                    }
                    
                    for(int i=3;i<3+length_of_raw_data;i++){
                      actual_data[i-3]=mssg.data[i];
                    }
                    int send_decoded_data=decode_OBD_data(mssg.data[2],actual_data);
                    if(xQueueSend(data_q,&send_decoded_data,pdMS_TO_TICKS(100))==pdTRUE){
                      Serial.printf("Sent the value :%d to BLE TASK\n",send_decoded_data);
                    }
                    else{
                      Serial.println("Queue is full");
                    }
                  }
                  else{
                    Serial.println("Wrong identifier or Mode");
                  }

          }
      Serial.println("--------------------------------");
    }
  }
  vTaskDelay(1000);
  }
}
void BLE_FN(void *pvParameters){
  while(1){
    int received;
    if(xQueueReceive(data_q,&received,pdMS_TO_TICKS(100)) == pdTRUE){
       Serial.printf("Received the value :%d in BLE TASK\n",received);
       uint8_t ble_format[2];
       ble_format[0]=received>>8;
       ble_format[1]=received&0xFF;
       pChar_1_N->setValue(ble_format,sizeof(ble_format));
       pChar_1_N->notify();
    }
       
  auto val = pChar_2_W->getValue();
  if(val.length()>0){
        if( val==start_session){
          start_can=false;
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
          //strcpy(hex_array,"");
          strcpy(hexChar1,"");   // Reinitialize zeroes and null to variable for next command/next random number generation 
          //strcpy(hexChar2,"");
          //strcpy(hexChar2,"$dvrnd,");
          pChar_2_W->setValue("");
          pChar_1_N->setValue("");
          //Serial.println("executed all the initialization");
        
      }
        else if((val[0]==0x27) && (prevention_flag)){
          bool flag=false;
          start_can=false;
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
            Serial.println("KEYS MATCH and STARTING CAN NOW.....");
            pChar_2_W->setValue(""); 
            start_can=true;  
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
    start_can=false;

  }
  }
  Serial.printf("Connedted Devices:%d\n",pServer_1->getConnectedCount());
    // pBLERes=pBLEScan->getResults();
    // Serial.printf("Connected Devices:%d\n",pBLERes->getCount());
    vTaskDelay(1000);
  }
} 

void loop() {
}
