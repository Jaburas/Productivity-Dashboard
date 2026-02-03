# ESP32 IoT Workspace Hub

A smart desk dashboard that integrates real-time weather data, NTP time synchronization, and a persistent task management system. This project features a built-in web server allowing for wireless control via a smartphone or PC.

## Key Features
* **NTP Time Sync:** Automatically fetches millisecond-accurate time from pool.ntp.org, eliminating the need for an external RTC module.
* **Live Weather:** Uses the OpenWeatherMap API to display current temperature and conditions for Ottawa.
* **Persistent Task List:** Implements LittleFS to store up to 10 tasks in the ESP32's flash memory so data is saved after power loss.
* **Web Portal:** A local web server handles HTTP POST requests to add or delete tasks from a mobile browser.
* **Smart Refresh:** Optimized drawing logic only refreshes the TFT screen when values change, preventing screen flicker.

## Hardware Components
* **MCU:** ESP32 DevKit V1
* **Display:** ST7789 240x320 TFT (GMT020-02)
* **Storage:** Internal Flash (via LittleFS)

## Pinout Configuration
| TFT Pin | ESP32 Pin | Function |
| :--- | :--- | :--- |
| CS | GPIO 5 | Chip Select |
| RST | GPIO 4 | Reset |
| DC | GPIO 2 | Data/Command |
| MOSI | GPIO 23 | Master Out Slave In |
| SCLK | GPIO 18 | Serial Clock |

## Setup Instructions
1. Open the project in the Arduino IDE.
2. Ensure you have the following libraries installed: Adafruit GFX, Adafruit ST7789, and ArduinoJson.
3. Replace the Wi-Fi and API placeholders in the code with your local credentials.
4. Set the **Partition Scheme** to **Default 4MB with SPIFFS** or **LittleFS** in the Tools menu.
5. Upload to your ESP32.
