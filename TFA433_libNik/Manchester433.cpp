#include "Manchester433.h"

#define HALF_BIT_US   500
#define SYNC_LOW_US   9000
#define HEADER_BITS   14

static uint8_t TX_PIN;

static byte lfsr_digest8(const uint8_t *message, unsigned n, byte gen, byte key) {
    byte sum = 0;
    for (unsigned k = 0; k < n; ++k) {
        byte data = message[k];
        for (int i = 7; i >= 0; --i) {
            if ((data >> i) & 1) sum ^= key;
            key = (key & 1) ? ((key >> 1) ^ gen) : (key >> 1);
        }
    }
    return sum ^ 0x64;
}

static void sendManchesterBit(bool bit) {
    if (bit) {
        digitalWrite(TX_PIN, LOW);
        delayMicroseconds(HALF_BIT_US);
        digitalWrite(TX_PIN, HIGH);
        delayMicroseconds(HALF_BIT_US);
    } else {
        digitalWrite(TX_PIN, HIGH);
        delayMicroseconds(HALF_BIT_US);
        digitalWrite(TX_PIN, LOW);
        delayMicroseconds(HALF_BIT_US);
    }
}

static void manchesterSync() {
    sendManchesterBit(0);
    sendManchesterBit(1);
    sendManchesterBit(0);
}

void manchesterInit(uint8_t pin) {
    TX_PIN = pin;
    pinMode(TX_PIN, OUTPUT);
    digitalWrite(TX_PIN, HIGH);
}

void sendFrame(const uint8_t *payload, uint8_t len) {
    manchesterSync();

    digitalWrite(TX_PIN, LOW);
    delayMicroseconds(SYNC_LOW_US);

    manchesterSync();

    for (int i = 0; i < HEADER_BITS; i++)
        sendManchesterBit(1);

    manchesterSync();

    for (int i = 0; i < len; i++)
        for (int b = 7; b >= 0; b--)
            sendManchesterBit(!((payload[i] >> b) & 1));

    uint8_t crc = lfsr_digest8(payload, len, 0x98, 0x3e);

    for (int b = 7; b >= 0; b--)
        sendManchesterBit(!((crc >> b) & 1));

    // 4) Finish with short low
    digitalWrite(TX_PIN, LOW);
}
