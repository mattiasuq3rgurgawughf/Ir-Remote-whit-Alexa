#include <WiFi.h>
#include <SinricPro.h>
#include <SinricProTV.h>
#include <SinricProSpeaker.h>
#include <SinricProThermostat.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <ir_Coolix.h>

// ─── WiFi ────────────────────────────────────────────
#define WIFI_SSID   "Mattias"
#define WIFI_PASS   "12345678"

// ─── SinricPro ───────────────────────────────────────
#define APP_KEY    "de9a3553-c39f-4450-8b4c-0290897f4018"
#define APP_SECRET "a31774a7-a48a-4086-b435-f0f15c57351c-c9c45d35-eb75-4d82-a8f0-93548b45983b"
#define TV_ID      "69f1a488a221de7879403ed2"
#define SPEAKER_ID "69fc006ef9b5f15fa7cf1fad"
#define AC_ID      "69fbff75baa50bf9bf2d9abd"

// ─── Pines IR ────────────────────────────────────────
#define IR_SEND_PIN 26
#define IR_RECV_PIN 34

// ─── Códigos NEC (TV LG) ─────────────────────────────
#define TV_POWER       0x20DF10EF
#define TV_VOLUME_UP   0x20DF40BF
#define TV_VOLUME_DOWN 0x20DFC03F
#define TV_MUTE        0x20DF906F

// ─── Códigos Panasonic (Parlante) ────────────────────
#define SPK_POWER    0x40040538BC81
#define SPK_VOL_UP   0x400405000401
#define SPK_VOL_DOWN 0x400405008481
#define SPK_MUTE     0x400405004C49
#define SPK_NEXT     0x40040538615C
#define SPK_PREV     0x40040538E1DC

// ─── Objetos IR ──────────────────────────────────────
IRsend      irsend(IR_SEND_PIN);
IRrecv      irrecv(IR_RECV_PIN);
IRCoolixAC  aircon(IR_SEND_PIN);
decode_results results;

// ─── Estado del Aire ─────────────────────────────────
bool   acPower       = false;
int    acTemperature = 24;
String acMode        = "COOL";


// ════════════════════════════════════════════════════
//                   CALLBACKS TV
// ════════════════════════════════════════════════════

bool onTVPowerState(const String &deviceId, bool &state) {
  Serial.printf("\n[TV] %s\n", state ? "ENCENDIDA" : "APAGADA");
  irsend.sendNEC(TV_POWER, 32);
  return true;
}

bool onTVVolume(const String &deviceId, int &volumeDelta, bool absoluteVolume) {
  bool subir = (volumeDelta > 0);
  Serial.printf("\n[TV] VOLUMEN %s\n", subir ? "SUBE" : "BAJA");
  irsend.sendNEC(subir ? TV_VOLUME_UP : TV_VOLUME_DOWN, 32);
  return true;
}

bool onTVMute(const String &deviceId, bool &mute) {
  Serial.println("\n[TV] MUTE");
  irsend.sendNEC(TV_MUTE, 32);
  return true;
}


// ════════════════════════════════════════════════════
//                CALLBACKS PARLANTE
// ════════════════════════════════════════════════════

bool onSpeakerPowerState(const String &deviceId, bool &state) {
  Serial.printf("\n[PARLANTE] %s\n", state ? "ENCENDIDO" : "APAGADO");
  if (state) {
    for (int i = 0; i < 5; i++) { irsend.sendPanasonic64(SPK_POWER); delay(10); yield(); }
  } else {
    irsend.sendPanasonic64(SPK_POWER);
  }
  return true;
}

bool onSpeakerVolume(const String &deviceId, int &volumeDelta, bool absoluteVolume) {
  bool subir = (volumeDelta > 0);
  Serial.printf("\n[PARLANTE] VOLUMEN %s\n", subir ? "SUBE" : "BAJA");
  irsend.sendPanasonic64(subir ? SPK_VOL_UP : SPK_VOL_DOWN);
  return true;
}

bool onSpeakerMute(const String &deviceId, bool &mute) {
  Serial.println("\n[PARLANTE] MUTE");
  irsend.sendPanasonic64(SPK_MUTE);
  return true;
}

bool onSpeakerMediaControl(const String &deviceId, String &control) {
  Serial.printf("\n[PARLANTE] MEDIA: %s\n", control.c_str());
  if      (control == "Next")     irsend.sendPanasonic64(SPK_NEXT);
  else if (control == "Previous") irsend.sendPanasonic64(SPK_PREV);
  return true;
}


// ════════════════════════════════════════════════════
//                  CALLBACKS AIRE
// ════════════════════════════════════════════════════

bool onACPowerState(const String &deviceId, bool &state) {
  acPower = state;
  Serial.printf("\n[AC] %s\n", state ? "ENCENDIDO" : "APAGADO");
  if (state) {
    aircon.on();
    aircon.setMode(kCoolixCool);
    aircon.setTemp(acTemperature);
  } else {
    aircon.off();
  }
  aircon.send();
  return true;
}

bool onACTargetTemperature(const String &deviceId, float &temperature) {
  if (temperature > 40) temperature = (temperature - 32) * 5.0 / 9.0; // °F → °C
  temperature = constrain(temperature, 17, 30);
  acTemperature = (int)temperature;
  Serial.printf("\n[AC] TEMPERATURA: %d°C\n", acTemperature);
  aircon.setTemp(acTemperature);
  aircon.send();
  return true;
}

bool onACMode(const String &deviceId, String &mode) {
  acMode = mode;
  Serial.printf("\n[AC] MODO: %s\n", mode.c_str());
  if      (mode == "COOL") aircon.setMode(kCoolixCool);
  else if (mode == "HEAT") aircon.setMode(kCoolixHeat);
  else if (mode == "DRY")  aircon.setMode(kCoolixDry);
  else                     aircon.setMode(kCoolixAuto);
  aircon.send();
  return true;
}


// ════════════════════════════════════════════════════
//                  SETUP & LOOP
// ════════════════════════════════════════════════════

void setupWiFi() {
  Serial.print("\nConectando a WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi conectado");
}

void setupSinricPro() {
  SinricProTV      &tv      = SinricPro[TV_ID];
  SinricProSpeaker &speaker = SinricPro[SPEAKER_ID];
  SinricProThermostat &ac   = SinricPro[AC_ID];

  tv.onPowerState(onTVPowerState);
  tv.onAdjustVolume(onTVVolume);
  tv.onMute(onTVMute);

  speaker.onPowerState(onSpeakerPowerState);
  speaker.onAdjustVolume(onSpeakerVolume);
  speaker.onMute(onSpeakerMute);
  speaker.onMediaControl(onSpeakerMediaControl);

  ac.onPowerState(onACPowerState);
  ac.onTargetTemperature(onACTargetTemperature);
  ac.onThermostatMode(onACMode);

  SinricPro.onConnected([]()    { Serial.println("\n✔ SinricPro conectado");    });
  SinricPro.onDisconnected([]() { Serial.println("\n✗ SinricPro desconectado"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
  Serial.begin(115200);
  irsend.begin();
  aircon.begin();
  irrecv.enableIRIn();
  setupWiFi();
  setupSinricPro();
  Serial.println("\nSistema iniciado ✓");
}

void loop() {
  SinricPro.handle();

  if (irrecv.decode(&results)) {
Serial.printf("\n[IR] Proto: %s | HEX: 0x%llX | Bits: %d\n",
  typeToString(results.decode_type).c_str(),
  (uint64_t)results.value,
  results.bits);
    irrecv.resume();
  }
}