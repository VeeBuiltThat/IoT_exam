#include "setting.h"
#include "config.h"

// =====================
// Global State Variables
// =====================

OperationMode current_Mode = WiFi_MODULE;
volatile Operation current  = DEACTIVE_MODE;
volatile bool ChangeRequested = false;

// =====================
// Button Debounce
// =====================

// volatile: written inside ISR, read in loop()
volatile unsigned long lastPressTime = 0;
const unsigned long debounceDelay    = 200;

// =====================
// Channel Groups
// =====================

const byte channelGroup_1[] = {2, 5, 8, 11};
const byte channelGroup_2[] = {26, 29, 32, 35};
const byte channelGroup_3[] = {80, 83, 86, 89};

const size_t channelGroup_1_size = sizeof(channelGroup_1);
const size_t channelGroup_2_size = sizeof(channelGroup_2);
const size_t channelGroup_3_size = sizeof(channelGroup_3);

// =====================
// Channel Definitions per Protocol
// =====================

const byte bluetooth_channels[] = {
  32,34,46,48,50,52,0,1,2,4,6,8,22,24,26,28,30,74,76,78,80
};
const byte ble_channels[]              = {2, 26, 80};
const byte WiFi_channels[]             = {1,2,3,4,5,6,7,8,9,10,11,12};
const byte usbWireless_channels[]      = {40, 50, 60};
const byte videoTransmitter_channels[] = {70, 75, 80};
const byte rc_channels[]               = {1, 3, 5, 7};
const byte zigbee_channels[]           = {11, 15, 20, 25};
const byte nrf24_channels[]            = {76, 78, 79};

// =====================
// Channel Array Sizes
// =====================

const size_t bluetooth_channels_size        = sizeof(bluetooth_channels);
const size_t ble_channels_size              = sizeof(ble_channels);
const size_t WiFi_channels_size             = sizeof(WiFi_channels);
const size_t usbWireless_channels_size      = sizeof(usbWireless_channels);
const size_t videoTransmitter_channels_size = sizeof(videoTransmitter_channels);
const size_t rc_channels_size               = sizeof(rc_channels);
const size_t zigbee_channels_size           = sizeof(zigbee_channels);
const size_t nrf24_channels_size            = sizeof(nrf24_channels);

// =====================
// NRF24 Radio Instance
// =====================

RF24 Radio(NRF_CE_PIN, NRF_CSN_PIN);

// =====================
// NRF Configuration Function
// =====================

void configureNrf(RF24 &radio) {
  if (!radio.begin()) {
    Serial.println("!! Radio init FAILED - check wiring");
    return;
  }
  radio.setAutoAck(false);
  radio.stopListening();
  radio.setRetries(0, 0);
  radio.setPALevel(RF24_PA_MAX, true);
  radio.setDataRate(RF24_2MBPS);
  radio.setCRCLength(RF24_CRC_DISABLED);
}
