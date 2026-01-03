
# 433 MHz TFA Dostmann 30.3227.02 – Receiver & Sender
Based on the TFAReceiver fork from spb2005 url=https://github.com/Spb2005/TFAReceiver. **Thanks spb2005, your work was a great help and a very good starting point!**
I added to that with a code to allow to send the exact dostmann frames that can be picked up by the TFAReceiver.

This project implements both a **receiver** and a **sender** for TFA Dostmann
30.3227.02 / 30.3208.x temperature & humidity sensors.  

This was my christmas 2025 holiday fun project. It kept me occupied for almost a full week, using not only the computer with the arduino ide, but also an oszilloscope and a logic analyser.
My goal was to add the original Dostmann senders into a homespan integration and add custom senders of my own making.
Quite some detective work and, I have to admit, some help from a KI. I learned that to use a KI in such a context, you have to question what it suggests, do a lot work and reasoning yourself to get the KI back on track. Only then I came up with a solution. It boils down to: **If you know what you do, if you could do it without a KI, then a KI can be a lot of help. Else don't bother!**

The optional goal is to read original TFA sensor frames and also **generate compatible
frames** so that an original TFA base station accepts them as if they were real sensors. Sp far the base station does not read the frames from this lib. I guess the bit timing is not close enough. But the receiver using a cheap RxB8 picks up both the Dostmann sender frames and our frames just fine.
For the sender I used a cheap 433Mhz sender nothing special. Important to add a 17.2cm Antenna to both sender and receiver! As receiver mcu I used a Nano r4 board or an ESP32-Wroom - both work fine (make sure to use PIO27 on the ESP!), for the sender a plain Nano.  
 **Beware the ESP32 can only tolerate 3.3V on the input pin, so a combination of a 1.8K and a 1k resistor to GND divides the 5V to 3.3V** The Nano R4 is happy with the 5V level.
 
## ✨ Features

### ✅ Receiver (Arduino Nano R4 / ESP32)
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

