#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);

  MCUCR = (1<<JTD);asc
  MCUCR = (1<<JTD);

  // Check if OLED is connected
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display_title_screen();

  String linesoftext[4] = {"Test", "Test2", "Test3", "Test4"};
  display_text("Please Select Domain", linesoftext, 16);      // Draw characters of the default font

  delay(5000);
  display.clearDisplay();
  display.display(); // Update display

}

void loop() {

}

