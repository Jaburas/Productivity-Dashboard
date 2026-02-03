#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <LittleFS.h> 
#include "time.h"

// TFT Pins for my GMT020-02 module
#define TFT_CS    5
#define TFT_RST   4 
#define TFT_DC    2
#define TFT_MOSI  23
#define TFT_SCLK  18

// Network Stuff
const char* ssid     = "GEAJAT";
const char* password = "Talyh103975";
const char* apiKey   = "cbda318b6717d0debed919ed666d0ede";
const char* city     = "Ottawa,CA";

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
WebServer server(80);

// Using a fixed array instead of vector (easier to manage for now)
String tasks[10]; 
int taskCount = 0;
float currentTemp = 0;
String weatherDesc = "Loading...";
int screenMode = 0; // 0 = Clock, 1 = Tasks
unsigned long lastWeatherUpdate = 0;
int lastMin = -1; // To track when to refresh the screen

// --- Save/Load using LittleFS ---
void saveTasks() {
  File f = LittleFS.open("/mytasks.txt", "w");
  if (f) {
    for (int i = 0; i < taskCount; i++) {
      f.println(tasks[i]);
    }
    f.close();
  }
}

void loadTasks() {
  if (!LittleFS.exists("/mytasks.txt")) return;
  File f = LittleFS.open("/mytasks.txt", "r");
  taskCount = 0;
  while (f.available() && taskCount < 10) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) {
      tasks[taskCount] = line;
      taskCount++;
    }
  }
  f.close();
}

// --- Webpage functions ---
void handleRoot() {
  String s = "<h1>My ESP32 Task Board</h1>";
  s += "<h3>Current Tasks:</h3><ul>";
  for(int i=0; i < taskCount; i++) {
    s += "<li>" + tasks[i] + " <a href='/del?id=" + String(i) + "'>[Delete]</a></li>";
  }
  s += "</ul>";
  s += "<form method='POST' action='/add'>New: <input name='t'><input type='submit'></form>";
  s += "<br><a href='/sw'>[Switch Screen Mode]</a>";
  server.send(200, "text/html", s);
}

void handleAdd() {
  if (server.hasArg("t") && taskCount < 10) {
    tasks[taskCount] = server.arg("t");
    taskCount++;
    saveTasks();
  }
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleDel() {
  if (server.hasArg("id")) {
    int id = server.arg("id").toInt();
    for (int i = id; i < taskCount - 1; i++) {
      tasks[i] = tasks[i+1]; // Shift them up
    }
    taskCount--;
    saveTasks();
  }
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleToggle() {
  screenMode = !screenMode;
  lastMin = -1; 
  tft.fillScreen(ST77XX_BLACK);
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void updateWeather() {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "&appid=" + String(apiKey) + "&units=metric";
    http.begin(url);
    if (http.GET() > 0) {
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, http.getString());
      currentTemp = doc["main"]["temp"];
      weatherDesc = doc["weather"][0]["description"].as<String>();
      weatherDesc.toUpperCase();
    }
    http.end();
  }
}

void setup() {
  Serial.begin(115200);
  
  if(!LittleFS.begin(true)) Serial.println("FS Error");
  loadTasks();

  tft.init(240, 320);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  tft.setCursor(10, 10);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("Connecting to WiFi...");
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(10, 10);
  tft.println("Connected!");
  tft.println(WiFi.localIP());
  delay(3000);

  configTime(-18000, 3600, "pool.ntp.org"); // Ottawa Time
  
  server.on("/", handleRoot);
  server.on("/add", handleAdd);
  server.on("/del", handleDel);
  server.on("/sw", handleToggle);
  server.begin();
  
  updateWeather();
}

void loop() {
  server.handleClient();

  // Update weather every 15 mins
  if (millis() - lastWeatherUpdate > 900000) {
    updateWeather();
    lastWeatherUpdate = millis();
  }

  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) return;

  // Only refresh screen if the minute changes to stop flickering
  if (timeinfo.tm_min != lastMin) {
    lastMin = timeinfo.tm_min;
    
    if (screenMode == 0) {
      tft.fillScreen(ST77XX_BLACK); // Clear for fresh draw
      tft.setCursor(40, 60);
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(7);
      tft.printf("%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
      
      tft.setCursor(20, 150);
      tft.setTextColor(ST77XX_ORANGE);
      tft.setTextSize(3);
      tft.print(currentTemp, 1); tft.print(" C");
      
      tft.setCursor(20, 190);
      tft.setTextColor(ST77XX_CYAN);
      tft.setTextSize(2);
      tft.println(weatherDesc);
    } else {
      tft.fillScreen(ST77XX_BLACK);
      tft.fillRect(0, 0, 320, 40, ST77XX_MAGENTA);
      tft.setCursor(10, 10);
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(3);
      tft.println("TASKS");

      for(int i=0; i < taskCount; i++) {
        tft.setCursor(10, 60 + (i * 25));
        tft.setTextColor(ST77XX_YELLOW);
        tft.setTextSize(2);
        tft.print("- ");
        tft.println(tasks[i]);
      }
    }
  }
}
