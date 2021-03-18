#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  pinMode(2, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.display();
  delay(2000);

  display.clearDisplay();
  start();

}

void start() {
  for (int i = 0; i < 16; i++) {
    display.drawLine(i, 0, i, 64, SSD1306_BLACK);
    display.drawLine(i, 24, i, 40, SSD1306_WHITE);
    display.display();
  }
}

int history[20];


int copter_x = 0;
int copter_y = 32;

int level_y = 24;
int level_x = 0;
int level_h = 16;
int level_n = 1;

int dy = 1;

void loop() {
  dy = 2*digitalRead(2);
  copter_y = copter_y + dy;
  copter_y = copter_y - 1;
  copter_x = copter_x + 1;
  if (copter_x == 128) {
    copter_x = 0;
  }
  level_x = level_x + 1;
  level_y = level_y + random(-level_n, level_n);
  if (level_y < 0) level_y = 0;
  if (level_y > 64 - level_h) level_y = 64 - level_h;
  if (level_x == 128) {
    level_x = 0;
    level_n = level_n + 1;
    if (level_h > 5) level_h = level_h - 2;
  }
  display.drawLine(level_x, 64, level_x, 0, SSD1306_BLACK);
  display.drawLine(level_x, 64 - level_y, level_x, 64 - (level_y + level_h), SSD1306_WHITE);
  display.drawPixel(copter_x, 64 - copter_y, SSD1306_BLACK);
  display.display();

}

void shiftLeft(int y) {
  for (int i = 19; i > 0; i--) {
    history[i + 1] = history[i];
    history[0] = y;
  }
}
