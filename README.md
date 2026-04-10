# 📡 RF-CLOWN v2.0.0

> A 2.4 GHz RF channel-hopping research tool built on Arduino + nRF24L01+

---

## ⚠️ Legal & Ethical Disclaimer

**RF-CLOWN is intended strictly for:**
- Controlled lab environments
- Personal RF research and education
- Testing interference resilience on networks **you own**

Intentionally jamming or interfering with radio communications — including WiFi, Bluetooth, Zigbee, RC, and BLE — is **illegal** in most jurisdictions worldwide, including under:

- **United States:** FCC Part 15 / 47 U.S.C. § 333
- **European Union:** Radio Equipment Directive (RED) 2014/53/EU
- **United Kingdom:** Wireless Telegraphy Act 2006

Violations can result in heavy fines, equipment seizure, and criminal prosecution. **Do not use this device on networks or spectrum you do not own or have explicit written permission to test.**

---

## Overview

RF-CLOWN is an Arduino-based tool that rapidly hops a 2.4 GHz nRF24L01+ radio transceiver across protocol-specific frequency channels. It supports 8 RF module profiles and is controlled via a serial terminal menu or a physical button.

Its primary use case is **evaluating 2.4 GHz spectrum congestion and interference resilience** in controlled environments.

---

## Hardware Requirements

| Component         | Details                          |
|-------------------|----------------------------------|
| Microcontroller   | Arduino Uno / Nano / compatible  |
| RF Module         | nRF24L01+ (2.4 GHz)              |
| Button            | Momentary push button            |
| LED               | Any standard LED + resistor      |

### Pin Mapping

| Function   | Pin |
|------------|-----|
| Button     | D3  |
| LED        | D2  |
| NRF CE     | D9  |
| NRF CSN    | D10 |

---

## Dependencies

- [RF24 Library](https://github.com/nRF24/RF24) by TMRh20

Install via Arduino Library Manager: `Sketch → Include Library → Manage Libraries → search "RF24"`

---

## File Structure

```
rf-clown/
├── main.ino     # Main sketch: setup, loop, menu, channel hopper
├── config.h         # Enums, pin definitions, extern declarations
├── config.cpp       # Global state, channel arrays, radio instance
├── setting.h        # RF24 include + configureNrf() declaration
└── README.md
```

---

## Supported RF Modules / Channel Profiles

| Mode            | Channels (nRF24 register values) | Approx. Frequency Range |
|-----------------|----------------------------------|--------------------------|
| WiFi            | 1–12                             | 2.401–2.412 GHz          |
| Bluetooth       | 0–80 (selected)                  | 2.400–2.480 GHz          |
| BLE             | 2, 26, 80                        | 2.402, 2.426, 2.480 GHz  |
| RC              | 1, 3, 5, 7                       | 2.401–2.407 GHz          |
| Video TX        | 70, 75, 80                       | 2.470–2.480 GHz          |
| USB Wireless    | 40, 50, 60                       | 2.440–2.460 GHz          |
| Zigbee          | 11, 15, 20, 25                   | 2.411–2.425 GHz          |
| NRF24           | 76, 78, 79                       | 2.476–2.479 GHz          |

---

## Usage

### Serial Terminal

Connect at **115200 baud**. On startup the menu is printed automatically.

```
========== RF-CLOWN ==========
Version v2.0.0

  1. WiFi
  2. Video TX
> 3. RC                [ACTIVE]
  4. BLE
  5. Bluetooth
  6. USB Wireless
  7. Zigbee
  8. NRF24
==============================
Up/Down arrows or 1-8 to select
Button to toggle ACTIVE/DEACTIVE
```

| Input          | Action                          |
|----------------|---------------------------------|
| `1`–`8`        | Select module profile           |
| `↑` / `↓`     | Navigate menu                   |
| `h` or `?`     | Reprint menu                    |
| Physical button | Toggle ACTIVE / DEACTIVE        |

### LED Indicator

| LED State     | Meaning       |
|---------------|---------------|
| Blinking fast | ACTIVE — hopping |
| Off           | DEACTIVE      |

### Hop Logging

Every channel hop is logged to serial:

```
[HOP #42] WiFi -> ch 6  (2406 MHz)
[HOP #43] WiFi -> ch 11  (2411 MHz)
```

---

## Configuration

Key constants in `main.ino`:

```cpp
#define HOP_INTERVAL 50   // ms between channel hops — lower = faster
```

Radio settings in `config.cpp` (`configureNrf()`):

```cpp
radio.setPALevel(RF24_PA_MAX, true);   // Max transmit power
radio.setDataRate(RF24_2MBPS);         // 2 Mbps data rate
radio.setAutoAck(false);               // No acknowledgements
radio.setCRCLength(RF24_CRC_DISABLED); // No CRC
```

---

## How to Observe Its Effect

For research and verification purposes, you can observe the device's RF activity using:

- **WiFi Analyzer** (Android) or **Network Radar** (iOS) — elevated noise floor, erratic RSSI
- **inSSIDer** / **Wireshark** — increased channel utilization across all 2.4 GHz channels
- **SDR (HackRF + GQRX)** — most accurate; shows broadband random impulse spikes across 2.400–2.480 GHz

The signature of this device is **random, broadband, short-duration spikes** — distinguishable from microwave interference (rhythmic) or co-channel networks (clean overlapping curves).

---

## Security Considerations

- This device operates on unlicensed 2.4 GHz ISM band spectrum.
- The nRF24L01+ has limited transmit power (~0 dBm max) — effective range is short.
- It does **not** decrypt, capture, or replay any RF traffic. It is a transmitter only.
- No network credentials, keys, or data are ever processed.
- Store and transport the device responsibly. In many jurisdictions, possession of jamming equipment with intent to use is itself an offence.

---

## License

This project is released for **educational and research purposes only**. The authors accept no liability for misuse. Use responsibly and legally.
