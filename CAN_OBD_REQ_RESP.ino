#include <driver/twai.h>
#include <M5AtomS3.h>

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

twai_message_t send_multi_req={
  .extd=0,
  .rtr=0,
  .ss=0,
  .self=0,
  .dlc_non_comp=0,
  .identifier=0x7DF,
  .data_length_code=2,
  .data={0x00,0x00},
};

uint8_t actual_data[4];
uint8_t count=0;


void setup() {

   Serial.begin(115200);
   AtomS3.begin(true);

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
}

long int convert_to_int(uint8_t* arr,uint8_t n){
  long int result=0;
  for(int i=0;i<n;i++){
    result|=arr[i]<<(8*(n-i-1));
  }

return result;

}



void loop() {

//memset(mssg.data,0,sizeof(mssg.data));

// if(twai_transmit(&engine_speed_req,pdMS_TO_TICKS(100))==ESP_OK){
//   //Serial.println("Message1 queued for transmission");
// }
// else{
//   Serial.println("failed to transmit Engine_speed_request");
// }
// delay(1000);
// if(twai_transmit(&vehical_speed_req,pdMS_TO_TICKS(100))==ESP_OK){
//   //Serial.println("Message1 queued for transmission");
// }
// else{
//   Serial.println("failed to transmit vehical_speed_request");
// }


if(twai_transmit(&send_multi_req,pdMS_TO_TICKS(100))==ESP_OK){
  
  Serial.printf("name:%s|PID data :0x%x,0x%x\n",OBD2_commands[count].name,OBD2_commands[count].pid[0],OBD2_commands[count].pid[1]);
  for(int i=0;i<2;i++){
  send_multi_req.data[i]=OBD2_commands[count].pid[i];
  }
  count++;
}
else{
  Serial.println("failed to transmit multi req");
}




if(twai_receive(&mssg,pdMS_TO_TICKS(100))==ESP_OK){
  Serial.printf("ID is %X\n", mssg.identifier);
  Serial.print("DATA:");

      if (!(mssg.rtr)) {
              for (int i = 0; i < mssg.data_length_code; i++) {
                  Serial.printf("%02X",mssg.data[i]);
              }
              Serial.println();
              if((0x7E7< mssg.identifier)&&(mssg.identifier<0x7F0) && ((0x40<mssg.data[1])&&(mssg.data[1]<0x4B))){

                Serial.println("OK identifier");
                uint8_t length_of_raw_data=mssg.data[0]-2;

                if(length_of_raw_data>4){
                  length_of_raw_data=4;
                }
                
                for(int i=3;i<3+length_of_raw_data;i++){
                  actual_data[i-3]=mssg.data[i];
                }

                int int_value=convert_to_int(actual_data,length_of_raw_data);
                Serial.printf("int Value of the data:%d\n",int_value);
              }
              else{
                Serial.println("Wrong identifier or Mode");
              }

      }
  Serial.println("--------------------------------");
}

delay(1000);  
    
}