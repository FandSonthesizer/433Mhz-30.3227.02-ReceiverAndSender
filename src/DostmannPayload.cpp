#include "DostmannPayload.h"

void buildDostmannBasePayload(
    uint8_t* payload,
    uint8_t type,      // usually 0x46 (70)
    uint8_t id,
    float tempC,       // temperature in °C
    uint8_t humidity,  // 0–100
    uint8_t channel,   // 1–3
    bool batteryOK
) {
    payload[0] = type;
    payload[1] = id;

    // Encode temperature for Dostmann base:
    // C = round((tempC + 40) * 18)
    float codeF = (tempC + 40.0f) * 18.0f;
    uint16_t C = (uint16_t)lroundf(codeF);  // 12-bit effective

    // Upper nibble of byte2: battery + channel
    uint8_t b2_high = 0;

    if (!batteryOK) {
        b2_high |= 0x80;          // bit7 = battery low
    }

    // channel: 1–3 → bits5–4 (matches real frames: 0x03 for ch1, 0x13 for ch2)
    b2_high |= ((channel - 1) & 0x03) << 4;

    // Lower nibble of byte2: upper 4 bits of C
    uint8_t b2_low = (C >> 8) & 0x0F;

    payload[2] = b2_high | b2_low;  // full byte2

    // Byte3: lower 8 bits of C
    payload[3] = C & 0xFF;

    // Byte4: humidity (as in real frames)
    payload[4] = humidity;

    // Depending on your RF stack, you may need to compute & append CRC elsewhere
}




