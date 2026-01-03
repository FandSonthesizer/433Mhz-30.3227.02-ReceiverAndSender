
/*
  ESP32-WROOM (NodeMCU-32S) + TFA 30.3208.02 + HomeSpan
  Style: TFAReceiver "BasicReceiver" (no TFAFrame object)
  Services: TemperatureSensor (CurrentTemperature), HumiditySensor (CurrentRelativeHumidity), BatteryService (StatusLowBattery)
  Notes:
   - Call receiver.checkBuf() in loop() and receiver.setTemperatureUnit(true) in setup().
   - Partition Scheme: Minimal SPIFFS (1.9MB APP) recommended for HomeSpan.
*/

#include <Arduino.h>
#include <TFAReceiver.h>           // d10i/Spb2005 library (BasicReceiver)
#include <HomeSpan.h>              // HomeSpan core
//#include <HomeSpan/Utils.h>

// -------------------------- USER CONFIG --------------------------

// RF input pin from your 433 MHz receiver
constexpr int RF_PIN = 27;

// Replace with the actual IDs you see in Serial from your BasicExample
constexpr uint16_t SENSOR_A_ID = 7;   // sensor #1
constexpr uint16_t SENSOR_B_ID = 43;   // sensor #2

// Map macros to your BasicExample getter names.
// If your example uses different names, modify ONLY these macros.
#define TFA_GET_ID()            receiver.getResult().id            // e.g., getId(), id(), sensorId()
#define TFA_GET_CHANNEL()       receiver.getResult().channel       // e.g., getChannel()
#define TFA_GET_TEMP_C()        receiver.getResult().temperature  // e.g., getTemperatureC(), getTemperature()
#define TFA_GET_HUMIDITY()      receiver.getResult().humidity      // e.g., getHumidity()
#define TFA_IS_BATTERY_OK()     receiver.getResult().battery      // e.g., isBatteryOK(), getBattery()

// ----------------------- TFA Receiver Instance --------------------
TFAReceiver receiver(RF_PIN);

// ---------------------- HomeSpan Accessory ------------------------
struct TFAAccessory : Service::AccessoryInformation {
  Service::TemperatureSensor *tempSvc;
  Characteristic::CurrentTemperature *curTemp;

  Service::HumiditySensor *humSvc;
  Characteristic::CurrentRelativeHumidity *curHum;

  Service::BatteryService *batSvc;               // optional
  Characteristic::StatusLowBattery *lowBatt;     // 0=Normal, 1=Low

  float lastTemp = NAN;
  float lastHum  = NAN;
  bool  lastLow  = false;

  TFAAccessory(const char *name, const char *model, const char *serial) :
    Service::AccessoryInformation() {

    new Characteristic::Identify();
    new Characteristic::Manufacturer("TFA/ESP32");
    new Characteristic::Model(model);
    new Characteristic::Name(name);
    new Characteristic::SerialNumber(serial);
    new Characteristic::FirmwareRevision("1.0.0");

    tempSvc = new Service::TemperatureSensor();
    curTemp = new Characteristic::CurrentTemperature(20.0);

    humSvc  = new Service::HumiditySensor();
    curHum  = new Characteristic::CurrentRelativeHumidity(50.0);

    batSvc  = new Service::BatteryService();
    lowBatt = new Characteristic::StatusLowBattery(0);
  }

  void updateFrom(float tempC, float humPct, bool batteryLow) {
    const float TEMP_EPS = 0.1f;     // push only meaningful changes
    const float HUM_EPS  = 1.0f;

    if (isnan(lastTemp) || fabsf(tempC - lastTemp) >= TEMP_EPS) {
      curTemp->setVal(tempC);        // notify HomeKit
      lastTemp = tempC;
    }
    if (isnan(lastHum) || fabsf(humPct - lastHum) >= HUM_EPS) {
      curHum->setVal(humPct);        // notify HomeKit
      lastHum = humPct;
    }
    if (batteryLow != lastLow) {
      lowBatt->setVal(batteryLow ? 1 : 0);
      lastLow = batteryLow;
    }
  }
};

// Bridge with two child accessories (one per sensor)
TFAAccessory *sensorAAcc = nullptr;
TFAAccessory *sensorBAcc = nullptr;

TFAAccessory* accForId(uint16_t id) {
  if (id == SENSOR_A_ID) return sensorAAcc;
  if (id == SENSOR_B_ID) return sensorBAcc;
  return nullptr;
}


void setup() {
  Serial.begin(115200);
  delay(250);

  // --- RF/TFA init (ESP32-safe pattern)
  receiver.begin();
  receiver.setTemperatureUnit(true);   // Celsius

  // --- Wi-Fi
  homeSpan.setWifiCredentials("YourSSid", "YourPasscode");

  // --- Use a valid HomeKit setup code format: NNN-NN-NNN
  homeSpan.setPairingCode("12345678");   

  // --- HomeSpan start: use Bridges category for a bridge device
  homeSpan.setLogLevel(1);
  homeSpan.begin(Category::Bridges, "TFA Bridge", "MyESP", "My-NodeMCU");

  // ------------------ Accessory #0: BRIDGE ------------------
  {
    new SpanAccessory();                          // create the bridge accessory (Accessory #0)

    // The bridge MUST have exactly one AccessoryInformation service
    new Service::AccessoryInformation();
    new Characteristic::Identify();
    new Characteristic::Manufacturer("TFA/ESP32");
    new Characteristic::Model("Bridge-30.3208.02");
    new Characteristic::Name("TFA Bridge");
    new Characteristic::SerialNumber("BRIDGE-0001");   // unique per device
    new Characteristic::FirmwareRevision("1.0.0");

    // IMPORTANT: DO NOT add Temperature/Humidity/Battery services to the bridge.
    // Child accessories will carry sensor services.
  }

  // ------------------ Accessory #1: SENSOR A ------------------
  {
    char serialBuf[16];
    snprintf(serialBuf, sizeof(serialBuf), "TFA-%u", SENSOR_A_ID);

    new SpanAccessory();  // child accessory
    sensorAAcc = new TFAAccessory("TFA Sensor A", "30.3208.02", serialBuf);
    // TFAAccessory() creates AccessoryInformation + Temperature + Humidity (+ Battery) services
  }

  // ------------------ Accessory #2: SENSOR B ------------------
  {
    char serialBuf[16];
    snprintf(serialBuf, sizeof(serialBuf), "TFA-%u", SENSOR_B_ID);

    new SpanAccessory();  // child accessory
       sensorBAcc = new TFAAccessory("TFA Sensor B", "30.3208.02", serialBuf);
  }

  Serial.println("HomeSpan + TFAReceiver ready. Pair from Home app → Add Accessory → More Options → enter 664-32-119.");

}


void loop() {
  homeSpan.poll();                 // HomeSpan’s main loop  [2](https://manualmachine.com/tfadostmann/433mhzoutdoortemperaturetransmitter/18724137-instruction-manual/)
  receiver.checkBuf();             // decode outside ISR per Spb2005 fork  [1](https://github.com/Spb2005/TFAReceiver)

  // BasicExample style: no TFAFrame; use receiver.available() + getters
  if (receiver.isAvailable()) {
    // >>> Replace these macro calls if your BasicExample uses different names <<<
    const uint16_t id      = TFA_GET_ID();
    const uint8_t  ch      = TFA_GET_CHANNEL();
    const float    tempC   = TFA_GET_TEMP_C();
    const float    humPct  = (float)TFA_GET_HUMIDITY();
    const bool     lowBatt = !TFA_IS_BATTERY_OK();

    if (auto *acc = accForId(id)) {
      acc->updateFrom(tempC, humPct, lowBatt);
      Serial.printf("Updated ID=%u CH=%u T=%.2f°C H=%.0f%% BattLow=%u\n",
                    id, ch, tempC, humPct, lowBatt ? 1 : 0);
    } else {
      Serial.printf("Unknown sensor ID=%u\n", id);
    }
  }

  delay(5);
}

