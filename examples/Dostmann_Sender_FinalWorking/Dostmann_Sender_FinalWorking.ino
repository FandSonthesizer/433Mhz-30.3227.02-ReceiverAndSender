/*
  Dostmann-compatible 433 MHz ASK sender
  Arduino Nano / ATmega328P
*/
#include "DostmannPayload.h"
#include "Manchester433.h"
#define TX_PIN 3

//uint8_t payload[DOSTMANN_PAYLOAD_BYTES] {0x46,0x2B,0x13,0x58,0xC}; // as received from original dostmann sender
uint8_t payload[DOSTMANN_PAYLOAD_BYTES];
// ---------------- Setup & Loop ----------------
void setup() {
  Serial.begin(115200);
  manchesterInit(3);
  float tempC = 40.2f;
  /*uint16_t rawTemp = (uint16_t)roundf((tempC + 40.0f) * 10.0f); 
  uint8_t temp_hi = (rawTemp >> 4) & 0xFF; 
  uint8_t temp_lo = rawTemp & 0x0F;
*/
uint16_t rawTemp = (uint16_t)roundf((tempC + 40.0f) * 10.0f); 
uint8_t temp_hi = (rawTemp >> 4) & 0xFF; 
uint8_t temp_lo = rawTemp & 0x0F;

 // uint16_t rawTemp = (uint16_t)roundf((tempC + 50.0f) * 10.0f);
  Serial.print("New rawTemp="); Serial.println(rawTemp);
  //uint8_t temp_hi = (rawTemp >> 4) & 0xFF;
  Serial.print("temp_hi="); Serial.println(temp_hi);
  //uint8_t temp_lo = rawTemp & 0x0F;
  Serial.print("temp_lo="); Serial.println(temp_lo);

  //buildDostmannPayloadA(payload, 71, 44, temp_hi, temp_lo, 40, 1, true); // geht channel 0-7
  
  buildDostmannBasePayload(payload, 46, 44, 1.5f, 44, 3, true);
}

void loop() {
  sendFrame(payload, DOSTMANN_PAYLOAD_BYTES);
  
  delay(3000);
}
