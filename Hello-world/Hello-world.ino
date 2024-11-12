#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialize the display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  // Clear the display buffer
  display.clearDisplay();

  // Set up and display "Hello World"
  display.setTextSize(2);       // Set text size
  display.setTextColor(WHITE);  // Set text color
  display.setCursor(10, 30);    // Position the text
  display.print("Hello World"); // Text to display
  display.display();            // Display it on the OLED
}

void loop() {
  // Nothing to do here for this simple example
}
