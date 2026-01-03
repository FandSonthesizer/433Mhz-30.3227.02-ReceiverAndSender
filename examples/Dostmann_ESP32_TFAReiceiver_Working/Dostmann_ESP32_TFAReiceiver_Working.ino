#include "TFAReceiver.h"

#define RXPIN 2 //Pin where 433Mhz receiver is connected, 27 on esp32, 2 on nano r4

TFAReceiver receiver(RXPIN); 

void setup() {
  Serial.begin(115200);
  receiver.begin();

  // optional: setting temperature unit. Default: Celsius
  //receiver.setTemperatureUnit(0); //Fahrenheit
  //receiver.setTemperatureUnit(1); //Celsius
}

void loop() {
  receiver.checkBuf();  // checks if buffer is valid

  if (receiver.isAvailable()) {
    TFAResult res = receiver.getResult();

    Serial.print("Type: "); Serial.print(res.type);
    Serial.print(" ID: "); Serial.print(res.id);
    Serial.print(" Battery OK: "); Serial.print(res.battery);
    Serial.print(" Channel: "); Serial.print(res.channel);
    Serial.print(" Temperature: "); Serial.print(res.temperature);
    Serial.print(" Humidity: "); Serial.println(res.humidity);
    Serial.println("-----------");
  }
}
