#include <driver/twai.h>
#include <M5AtomS3.h>

twai_message_t mssg;
twai_message_t tx1_mssg={
  .extd=1,
  .rtr=0,
  .ss=0,
  .self=0,
  .dlc_non_comp=0,
  .identifier=0x12AA,
  .data_length_code=4,
  .data={0xA,0xB,0xC,0x12},
};
twai_message_t tx2_mssg={
  .extd=1,
  .rtr=0,
  .ss=0,
  .self=0,
  .dlc_non_comp=0,
  .identifier=0xAF,
  .data_length_code=8,
  .data={0x1,0x22,0x3E,0x48,0xAA,0x32,0x76,0xF1},
};
twai_message_t tx3_mssg={
  .extd=0,
  .rtr=0,
  .ss=0,
  .self=0,
  .dlc_non_comp=0,
  .identifier=0xC,
  .data_length_code=1,
  .data={0x1},
};
int count=0;


void setup() {
   Serial.begin(115200);
   AtomS3.begin(true);

   twai_general_config_t g_config=TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_2,GPIO_NUM_1, TWAI_MODE_NORMAL);
   twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
   twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  // // twai_general_config_t g_config;

  // g_config.mode=TWAI_MODE_NORMAL;


  
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
}



void loop() {

if(twai_receive(&mssg,pdMS_TO_TICKS(10))==ESP_OK){
  Serial.println("Message received");
  Serial.printf("ID is %X\n", mssg.identifier);
  Serial.print("DATA:");
  if (!(mssg.rtr)) {
    for (int i = 0; i < mssg.data_length_code; i++) {
        Serial.printf("%02X",mssg.data[i]);
    }

}
Serial.println();
memset(mssg.data,0,sizeof(mssg.data));
}

if(twai_transmit(&tx1_mssg,pdMS_TO_TICKS(100))==ESP_OK){
  //Serial.println("Message1 queued for transmission");
}
else{
  Serial.println("failed to transmit 1");
}
if(twai_transmit(&tx2_mssg,pdMS_TO_TICKS(100))==ESP_OK){
  //Serial.println("Message2 queued for transmission");
}
else{
  Serial.println("failed to transmit 2");
}
if(twai_transmit(&tx3_mssg,pdMS_TO_TICKS(100))==ESP_OK){
 // Serial.println("Message3 queued for transmission");
  tx3_mssg.data[0]=count+1;
  count++;
}
else{
  Serial.println("failed to transmit3");
}




// if (mssg.extd) {
//     Serial.println("Message is in Extended Format");
// } else {
//     Serial.println("Message is in Standard Format");
// }



delay(1000);  
    
}