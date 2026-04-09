#include "setting.h"
#include "config.h"
#include <Arduino.h>

unsigned long ledTimer  = 0;
bool ledState           = false;

// How often to hop channels (ms) — lower = faster hopping
#define HOP_INTERVAL 50

unsigned long lastHopTime = 0;
unsigned long hopCount    = 0;

// ── Button ISR ──────────────────────────────────────────────────────────────
void handleButton() {
  unsigned long now = millis();
  if (now - lastPressTime > debounceDelay) {
    current = (current == DEACTIVE_MODE) ? ACTIVE_MODE : DEACTIVE_MODE;
    lastPressTime = now;
  }
}

// ── Radio init ───────────────────────────────────────────────────────────────
void initialize_Radios() {
  if (current == ACTIVE_MODE) {
    configureNrf(Radio);
  } else {
    Radio.powerDown();
    delay(100);
  }
}

// ── Menu ─────────────────────────────────────────────────────────────────────
static const char* kMenuLabels[] = {
  "WiFi", "Video TX", "RC", "BLE", "Bluetooth", "USB Wireless", "Zigbee", "NRF24"
};
static const int kMenuCount = sizeof(kMenuLabels) / sizeof(kMenuLabels[0]);

static int menuIndexFromMode(OperationMode m) {
  switch (m) {
    case WiFi_MODULE:         return 0;
    case VIDEO_TX_MODULE:     return 1;
    case RC_MODULE:           return 2;
    case BLE_MODULE:          return 3;
    case Bluetooth_MODULE:    return 4;
    case USB_WIRELESS_MODULE: return 5;
    case ZIGBEE_MODULE:       return 6;
    case NRF24_MODULE:        return 7;
    default:                  return 0;
  }
}

static OperationMode modeFromMenuIndex(int idx) {
  switch (idx) {
    case 0: return WiFi_MODULE;
    case 1: return VIDEO_TX_MODULE;
    case 2: return RC_MODULE;
    case 3: return BLE_MODULE;
    case 4: return Bluetooth_MODULE;
    case 5: return USB_WIRELESS_MODULE;
    case 6: return ZIGBEE_MODULE;
    case 7: return NRF24_MODULE;
    default: return WiFi_MODULE;
  }
}

static const char* labelFromMode(OperationMode m) {
  return kMenuLabels[menuIndexFromMode(m)];
}

void printMenuToSerial() {
  Serial.println();
  Serial.println("========== RF-CLOWN ==========");
  Serial.println("Version v2.0.0");
  Serial.println();

  int focus = menuIndexFromMode(current_Mode);
  for (int i = 0; i < kMenuCount; i++) {
    Serial.print((i == focus) ? "> " : "  ");
    Serial.print(i + 1);
    Serial.print(". ");
    Serial.print(kMenuLabels[i]);
    if (i == focus)
      Serial.println((current == ACTIVE_MODE) ? "  [ACTIVE]" : "  [DEACTIVE]");
    else
      Serial.println();
  }

  Serial.println("==============================");
  Serial.println("Up/Down arrows or 1-8 to select");
  Serial.println("Button to toggle ACTIVE/DEACTIVE");
  Serial.println();
}

// ── Serial input parser ───────────────────────────────────────────────────────
void handleSerialInput() {
  if (!Serial.available()) return;

  int c = Serial.read();

  if (c == 27) {
    delay(5);
    if (Serial.available() && Serial.read() == '[') {
      if (Serial.available()) {
        int arrow = Serial.read();
        int focus = menuIndexFromMode(current_Mode);
        if (arrow == 'A') {
          focus = (focus - 1 + kMenuCount) % kMenuCount;
          current_Mode = modeFromMenuIndex(focus);
          printMenuToSerial();
        } else if (arrow == 'B') {
          focus = (focus + 1) % kMenuCount;
          current_Mode = modeFromMenuIndex(focus);
          printMenuToSerial();
        }
      }
    }
    return;
  }

  if (c >= '1' && c <= '8') {
    current_Mode = modeFromMenuIndex(c - '1');
    printMenuToSerial();
    return;
  }

  if (c == 'h' || c == '?') {
    printMenuToSerial();
  }
}

// ── Channel hop + log ────────────────────────────────────────────────────────
void hopChannel() {
  byte channel = 0;
  switch (current_Mode) {
    case BLE_MODULE:          channel = ble_channels[random(0, ble_channels_size)];                          break;
    case Bluetooth_MODULE:    channel = bluetooth_channels[random(0, bluetooth_channels_size)];              break;
    case WiFi_MODULE:         channel = WiFi_channels[random(0, WiFi_channels_size)];                        break;
    case USB_WIRELESS_MODULE: channel = usbWireless_channels[random(0, usbWireless_channels_size)];          break;
    case VIDEO_TX_MODULE:     channel = videoTransmitter_channels[random(0, videoTransmitter_channels_size)];break;
    case RC_MODULE:           channel = rc_channels[random(0, rc_channels_size)];                            break;
    case ZIGBEE_MODULE:       channel = zigbee_channels[random(0, zigbee_channels_size)];                    break;
    case NRF24_MODULE:        channel = nrf24_channels[random(0, nrf24_channels_size)];                      break;
  }

  Radio.setChannel(channel);
  /* hopCount++;

  // Log every hop to serial
  Serial.print("[HOP #");
  Serial.print(hopCount);
  Serial.print("] ");
  Serial.print(labelFromMode(current_Mode));
  Serial.print(" -> ch ");
  Serial.print(channel);
  Serial.print("  (");
  Serial.print(2400 + channel);
  Serial.println(" MHz)");*/
}

// ── Setup ────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println(">> setup start");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.println(">> pins OK");

  pinMode(PIN_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN), handleButton, FALLING);
  Serial.println(">> interrupt attached");

  Serial.println(">> calling initialize_Radios...");
  initialize_Radios();
  Serial.println(">> radios OK");

  printMenuToSerial();
}

// ── Loop ─────────────────────────────────────────────────────────────────────
void loop() {
  static Operation     lastActivity = DEACTIVE_MODE;
  static OperationMode lastMode     = WiFi_MODULE;

  handleSerialInput();

  // Atomically snapshot volatile current
  Operation snap;
  noInterrupts();
  snap = current;
  interrupts();

  // React to state or mode change
  if (snap != lastActivity || current_Mode != lastMode) {
    hopCount = 0; // reset hop counter on mode change
    initialize_Radios();
    printMenuToSerial();
    lastActivity = snap;
    lastMode     = current_Mode;
  }

  if (snap == ACTIVE_MODE) {
    // Blink LED
    if (millis() - ledTimer > 300) {
      ledTimer = millis();
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }

    // Hop channel at fixed interval
    if (millis() - lastHopTime > HOP_INTERVAL) {
      lastHopTime = millis();
      hopChannel();
    }

  } else {
    digitalWrite(LED_PIN, LOW);
  }
}
