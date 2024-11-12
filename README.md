# Bitcoin Price Tracker with ESP32 and OLED Display

A beginner-friendly project that uses an ESP32 microcontroller and an SSD1306 OLED display to fetch and display real-time Bitcoin prices from the CoinGecko API.


![2](https://github.com/user-attachments/assets/ee2048d0-6321-4df8-91a1-df58e95eb8ac)


## Features
- 🌟 Displays "Hello World" for initial setup.
- 🌐 Connects to WiFi to fetch live Bitcoin prices.
- 📊 Parses JSON data from the CoinGecko API.
- 💡 Displays Bitcoin prices on a 128x64 OLED screen with smooth updates.

## Requirements
- **Hardware:**
  - ESP32 microcontroller
  - SSD1306 OLED display (128x64)
- **Software:**
  - Arduino IDE
  - Required Libraries:
    - `WiFi.h`
    - `HTTPClient.h`
    - `ArduinoJson.h`
    - `Adafruit_GFX.h`
    - `Adafruit_SSD1306.h`

## Getting Started
1. **Clone this repository:**
   ```bash
   git clone https://github.com/yourusername/bitcoin-price-tracker.git
   cd bitcoin-price-tracker
