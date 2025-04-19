#include <SPI.h>
#include <mcp_can.h>

#define CAN_CS 10  // Chip Select pin for MCP2515
MCP_CAN CAN(CAN_CS);

void setup() {
  Serial.begin(115200);
  while (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK) {
    Serial.println("CAN BUS init failed!");
    delay(100);
  }
  Serial.println("CAN BUS ready!");
}

void loop() {
  byte data[8] = {1, 2, 3, 4, 5, 6, 7, 8}; // Example data
  CAN.sendMsgBuf(0x100, 0, 8, data); // Send data to ID 0x100
  delay(1000);
}
