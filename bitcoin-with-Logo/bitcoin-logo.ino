
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

const unsigned long updateInterval = 5 * 60 * 1000;
unsigned long lastUpdate = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Adjusted Y position for better centered display without 24h indicator
const int PRICE_Y_START = 80;
const int PRICE_Y_END = 32;  // Slightly adjusted for better vertical centering
const int ANIMATION_STEPS = 10;
const int ANIMATION_DELAY = 50;

void drawModernSplash() {
  display.clearDisplay();
  
  int centerX = SCREEN_WIDTH / 2;
  int centerY = SCREEN_HEIGHT / 2 - 5;
  
  for (int i = 0; i < 2; i++) {
    display.drawLine(centerX - 8 + i, centerY - 12, centerX - 8 + i, centerY + 12, WHITE);
  }
  
  display.drawRoundRect(centerX - 8, centerY - 12, 16, 12, 4, WHITE);
  display.drawRoundRect(centerX - 8, centerY, 16, 12, 4, WHITE);
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(centerX - 18, centerY + 20);
  display.println("BITCOIN");
  
  for (int i = 0; i < 3; i++) {
    display.display();
    display.fillCircle(centerX - 12 + i * 12, centerY + 30, 2, WHITE);
    delay(300);
  }
  
  display.display();
  delay(1000);
}

void animatePrice(float price, float prevPrice) {
  int currentY = PRICE_Y_START;
  float displayPrice = prevPrice;
  float priceStep = (price - prevPrice) / ANIMATION_STEPS;
  
  while (currentY > PRICE_Y_END) {
    display.clearDisplay();
    
    // Header with live indicator
    display.drawFastHLine(0, 0, SCREEN_WIDTH, WHITE);
    display.setTextSize(1);
    display.setCursor(4, 4);
    display.print("BTC/USD");
    
    // Animated dot for "live" indicator
    if ((millis() / 500) % 2) {
      display.fillCircle(SCREEN_WIDTH - 4, 4, 2, WHITE);
    }
    
    // Animated price
    displayPrice += priceStep;
    display.setTextSize(2);
    display.setCursor(4, currentY);
    display.print("$");
    display.print(displayPrice, 2);
    
    // Progress bar
    int progress = map(millis() % updateInterval, 0, updateInterval, 0, SCREEN_WIDTH);
    display.drawFastHLine(0, SCREEN_HEIGHT - 1, progress, WHITE);
    
    display.display();
    currentY -= (PRICE_Y_START - PRICE_Y_END) / ANIMATION_STEPS;
    delay(ANIMATION_DELAY);
  }
}

void displayPrice(float price) {
  display.clearDisplay();
  
  // Top bar with BTC symbol
  display.drawFastHLine(0, 0, SCREEN_WIDTH, WHITE);
  display.setTextSize(1);
  display.setCursor(4, 4);
  display.print("BTC/USD");
  
  // Live indicator
  if ((millis() / 500) % 2) {
    display.fillCircle(SCREEN_WIDTH - 4, 4, 2, WHITE);
  }
  
  // Main price display - now more centered without 24h indicator
  display.setTextSize(2);
  display.setCursor(4, PRICE_Y_END);
  display.print("$");
  display.print(price, 2);
  
  // Progress bar for update interval
  int progress = map(millis() % updateInterval, 0, updateInterval, 0, SCREEN_WIDTH);
  display.drawFastHLine(0, SCREEN_HEIGHT - 1, progress, WHITE);
  
  display.display();
}

void displayError(const char* error) {
  display.clearDisplay();
  
  int centerX = SCREEN_WIDTH / 2;
  int centerY = SCREEN_HEIGHT / 2 - 10;
  display.drawLine(centerX - 5, centerY - 5, centerX + 5, centerY + 5, WHITE);
  display.drawLine(centerX + 5, centerY - 5, centerX - 5, centerY + 5, WHITE);
  
  display.setTextSize(1);
  display.setCursor(centerX - 20, centerY + 10);
  display.println(error);
  
  if ((millis() / 500) % 2) {
    display.setCursor(centerX - 30, centerY + 25);
    display.println("Retrying...");
  }
  
  display.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  drawModernSplash();
  
  WiFi.begin(ssid, password);
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Connecting...");
  display.display();
  
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void fetchAndDisplayPrice() {
  static float previousPrice = 0;
  
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
        
        if(previousPrice != 0) {
          animatePrice(bitcoinPrice, previousPrice);
        }
        
        displayPrice(bitcoinPrice);
        previousPrice = bitcoinPrice;
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
