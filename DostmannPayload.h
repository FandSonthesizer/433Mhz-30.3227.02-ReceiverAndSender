#ifndef DOSTMANN_PAYLOAD_H
#define DOSTMANN_PAYLOAD_H

#include <Arduino.h>

#define DOSTMANN_PAYLOAD_BYTES 5

void buildDostmannBasePayload(
    uint8_t *payload,
    uint8_t type,
    uint8_t id,
    float tempC,
    uint8_t humidity,
    uint8_t channel,
    bool batteryOK
);

void buildDostmannPayloadA(
    uint8_t *payload,
    uint8_t type,
    uint8_t id,
    int tHigh,
    int tLow,
    uint8_t humidity,
    uint8_t channel,
    bool batteryOK
);
#endif
