A smart desk dashboard that integrates real-time weather data, NTP time synchronization, and a persistent task management system. 
This project features a built-in web server allowing for wireless control via a smartphone or PC.
Key Features

NTP Time Sync: Automatically fetches millisecond-accurate time from pool.ntp.org, eliminating the need for an external RTC module.
Live Weather: Uses the OpenWeatherMap API to display current temperature and conditions for Ottawa.
Persistent Task List: Implements LittleFS to store up to 10 tasks in the ESP32's flash memory so data is saved after power loss.
Web Portal: A local web server handles HTTP POST requests to add or delete tasks from a mobile browser.
Smart Refresh: Optimized drawing logic only refreshes the TFT screen when values change, preventing screen flicker.Hardware
Components
MCU: ESP32 DevKit V1
Display: ST7789 240x320 TFT (GMT020-02)
Storage: Internal Flash (via LittleFS)

Setup Instructions:

Open the project in the Arduino IDE.
Ensure you have the following libraries installed: Adafruit GFX, Adafruit ST7789, and ArduinoJson.
Replace the Wi-Fi and API placeholders in the code with your local credentials.
Set the "Partition Scheme" to "Default 4MB with SPIFFS" or "LittleFS" in the Tools menu.
Upload to your ESP32.
