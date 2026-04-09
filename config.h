#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Enumeration of supported RF operation modules
enum OperationMode {
  WiFi_MODULE,
  VIDEO_TX_MODULE,
  RC_MODULE,
  BLE_MODULE,
  Bluetooth_MODULE,
  USB_WIRELESS_MODULE,
  ZIGBEE_MODULE,
  NRF24_MODULE
};

// System state
enum Operation {
  DEACTIVE_MODE,
  ACTIVE_MODE
};

// =====================
// Hardware Pin Mapping
// =====================

#define PIN_BTN    3
#define LED_PIN    2
#define NRF_CE_PIN  9
#define NRF_CSN_PIN 10

// =====================
// Global State Variables
// =====================

extern OperationMode current_Mode;
extern volatile Operation current;
extern volatile bool ChangeRequested;

// All ISR-touched variables must be volatile
extern volatile unsigned long lastPressTime;
extern const unsigned long debounceDelay;

// =====================
// Channel Groups
// =====================

extern const byte channelGroup_1[];
extern const byte channelGroup_2[];
extern const byte channelGroup_3[];

extern const size_t channelGroup_1_size;
extern const size_t channelGroup_2_size;
extern const size_t channelGroup_3_size;

// =====================
// Channel Lists per Module
// =====================

extern const byte bluetooth_channels[];
extern const byte ble_channels[];
extern const byte WiFi_channels[];
extern const byte usbWireless_channels[];
extern const byte videoTransmitter_channels[];
extern const byte rc_channels[];
extern const byte zigbee_channels[];
extern const byte nrf24_channels[];

// =====================
// Channel Array Sizes
// =====================

extern const size_t bluetooth_channels_size;
extern const size_t ble_channels_size;
extern const size_t WiFi_channels_size;
extern const size_t usbWireless_channels_size;
extern const size_t videoTransmitter_channels_size;
extern const size_t rc_channels_size;
extern const size_t zigbee_channels_size;
extern const size_t nrf24_channels_size;

#endif
