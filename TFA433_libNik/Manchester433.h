#ifndef MANCHESTER_433_H
#define MANCHESTER_433_H

#include <Arduino.h>

void manchesterInit(uint8_t txPin);
void sendFrame(const uint8_t *payload, uint8_t len);

#endif
