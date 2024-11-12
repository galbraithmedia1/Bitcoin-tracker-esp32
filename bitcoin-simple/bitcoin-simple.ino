#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

const char* ssid = "";
const char* password = "";
const char* url = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd";

const unsigned long updateInterval = 5 * 60 * 1000;  // 5 minutes
unsigned long lastUpdate = 0;


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin();

  // Initialize the display
 if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.setTextColor(WHITE);
  WiFi.begin(ssid, password);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(4, 25);
  display.print("Connecting...");
  display.display();

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}


void displayPrice(float price) {
  display.clearDisplay();
  
  // Header
  display.setTextSize(1);
  display.setCursor(4, 4);
  display.print("BTC/USD");
  
  // Price display
  display.setTextSize(2);
  display.setCursor(4, 25);
  display.print("$");
  display.print(price, 2);
  
  display.display();
}


void displayError(const char* error) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(4, 25);
  display.print(error);
  display.display();
}

void fetchAndDisplayPrice() {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(url);
    
    int httpResponseCode = http.GET();
    
    if(httpResponseCode > 0) {
      String payload = http.getString();
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, payload);
      
      if(!error) {
        float bitcoinPrice = doc["bitcoin"]["usd"];
        lastUpdate = millis();
        displayPrice(bitcoinPrice);
      } else {
        displayError("JSON Error");
      }
    } else {
      displayError("HTTP Error");
    }
    
    http.end();
  } else {
    displayError("WiFi Error");
  }
}



void loop() {
  unsigned long currentMillis = millis();
  
  if(currentMillis - lastUpdate >= updateInterval || lastUpdate == 0) {
    fetchAndDisplayPrice();
  }
  
  delay(1000);
}

