
# 433 MHz TFA Dostmann 30.3227.02 – Receiver & Sender
Based on the TFAReceiver fork from spb2005 url=https://github.com/Spb2005/TFAReceiver 
I expanded on that by adding a code to allow to send the exact dostman frames that can be picked up by the TFAReceiver.

This project implements both a **receiver** and a **sender** for TFA Dostmann
30.3227.02 / 30.3208.x temperature & humidity sensors.  

My goal was to add the original Dostmann senders into a homespan integration and add custom sender of my own making.

The optional goal is to read original TFA sensor frames and also **generate compatible
frames** so that an original TFA base station accepts them as if they were real sensors. Sp far the base station does not read the frames from this lib. I guess the bit timing is not close enough. But the receiver using a cheap RxB8 picks up the Dostmann sender frames and our frames just fine.
For the sender I used a cheap 433Mhz sender nothing special. Important to add a 17.2cm Antenna to both sender and receiver!

## ✨ Features

### ✅ Receiver (ESP32)
- Uses a 433 MHz ASK/OOK receiver module
- Decodes Manchester‑encoded frames
- Fully compatible with TFA 30.3208 / 30.3227.02 protocol
- Extracts:
  - Temperature  
  - Humidity  
  - Battery status  
  - Rolling code / Sensor ID  
- Based on (and tested with) the `Spb2005/d10i` TFAReceiver library

### ✅ Sender (Arduino Nano / ESP32)
- Generates **valid Dostmann frames**
- Implements the correct **Manchester encoding**
- Correct CRC using LFSR with XOROUT `0x64`
- Bit order and inversion matched to original sensors
- Can send custom sensor data (e.g., from your own sensor or hard‑coded)

### ✅ Optional: Apple HomeKit Integration via HomeSpan
- The ESP32 receiver can act as a HomeKit bridge accessory
- Exposes:
  - Temperature  
  - Humidity  
  - Battery status  
- Works with iPhone, HomePod, and Home App

---

## 📡 Protocol Details

The TFA 30.3227.02 protocol uses:

- 433.92 MHz OOK (ASK) modulation  
- Manchester encoding  
- 36‑bit payload + 8‑bit CRC  
- Temperature encoded in 0.1°C steps  
- Humidity as 0–100%  
- CRC: L- CRC: LFSR polynomial + XOROUT 0x64 (required for correct checksum)

