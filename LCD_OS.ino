#include <Elegoo_GFX.h>
#include <Elegoo_TFTLCD.h>
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
void initLCD() {
  tft.reset();

  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  tft.setTextSize(1);
}

#include <Arduino_FreeRTOS.h>
#include <queue.h>

QueueHandle_t comQueue;

struct com{
  int type, ip1, ip2, ip3, ip4, ip5;
  String sp1;
};

void setup() {
  comQueue = xQueueCreate(10, sizeof(com));
  xTaskCreate(TaskDisplay, "Display", 128, NULL, 3, NULL);
  xTaskCreate(TaskAppOne, "App One", 128, NULL, 1, NULL);
  xTaskCreate(TaskAppTwo, "App Two", 128, NULL, 1, NULL);
}

void loop() {}

void TaskAppOne(void *pvParameters) {
  (void) pvParameters;

  com c;

  for (;;) {
    c.type = 4;
    c.ip1 = 10;
    c.ip2 = 10;
    c.ip3 = 50;
    c.ip4 = 35;
    c.ip5 = WHITE;
    xQueueSend(comQueue, &c, portMAX_DELAY);
  }
}

void TaskAppTwo(void *pvParameters) {
  (void) pvParameters;

  com c;

  for (;;) {
    c.type = 4;
    c.ip1 = 100;
    c.ip2 = 10;
    c.ip3 = 50;
    c.ip4 = 35;
    c.ip5 = WHITE;
    xQueueSend(comQueue, &c, portMAX_DELAY);
  }
}

void TaskDisplay(void * pvParameters) {
  (void) pvParameters;
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.setTextSize(2);
  tft.fillScreen(BLACK);
  com c;

  for (;;)
  {
    if (xQueueReceive(comQueue, &c, portMAX_DELAY) == pdPASS) {
      if (c.type == 1) {
        tft.fillScreen(c.ip1);
      } else if (c.type == 2) {
        tft.setCursor(c.ip1, c.ip2);
        tft.setTextColor(c.ip3);
        tft.print(c.sp1);
      } else if (c.type == 3) {
        tft.drawPixel(c.ip1, c.ip2, c.ip3);
      } else if (c.type == 4) {
        tft.drawRect(c.ip1, c.ip2, c.ip3, c.ip4, c.ip5);
      } else if (c.type == 5) {
        tft.fillRect(c.ip1, c.ip2, c.ip3, c.ip4, c.ip5);
      }
    }
  }
}
