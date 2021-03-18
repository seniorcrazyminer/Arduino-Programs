#include <Arduino_FreeRTOS.h>
#include <queue.h>

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

QueueHandle_t comQueue;

struct com {
  int type, ip1, ip2, ip3, ip4, ip5;
  String sp1;
};


void setup() {

  comQueue = xQueueCreate(10, sizeof(com));

  xTaskCreate(TaskDisplay, "Display", 128, NULL, 3, NULL);
  xTaskCreate(TaskOne, "Task One", 128, NULL, 0, NULL);
  xTaskCreate(TaskTwo, "Task Two", 128, NULL, 0, NULL);
  xTaskCreate(TaskFive, "Task Five", 128, NULL, 0, NULL);

}

void loop() {}

void TaskOne(void *pvParameters) {
  (void) pvParameters;
  com c;

  int cur = 0;
  int i = 0;
  String msg = "Happy Valentines";

  for (;;) {
    i = random(0, msg.length());
    c.type = 2;
    c.ip1 = 10 * i;
    c.ip2 = 10;
    if (random(0, 100) < 50) {
      if (random(0, 100) < 50) {
        c.ip3 = MAGENTA;
      } else {
        c.ip3 = WHITE;
      }
    } else {
      c.ip3 = RED;
    }
    c.sp1 = msg[i];
    xQueueSend(comQueue, &c, portMAX_DELAY);
  }
}

void TaskTwo(void *pvParameters) {
  (void) pvParameters;
  com c;

  int cur = 0;
  int i = 0;
  String msg = "Day Addie!";

  for (;;) {
    i = random(0, msg.length());
    c.type = 2;
    c.ip1 = 10 * i;
    c.ip2 = 30;
    if (random(0, 100) < 50) {
      if (random(0, 100) < 50) {
        c.ip3 = MAGENTA;
      } else {
        c.ip3 = WHITE;
      }
    } else {
      c.ip3 = RED;
    }
    c.sp1 = msg[i];
    xQueueSend(comQueue, &c, portMAX_DELAY);
  }
}

void TaskFive(void *pvParameters) {
  (void) pvParameters;
  com c;

  int _x[] = {1, 2, 3, 4, 4, 3, 2, 1, 0, 0};
  int _y[] = {0, 1, 0, 1, 2, 3, 4, 3, 2, 1};

  int x = random(0, 240);
  int y = random(70, 320);

  int idx = 0;

  c.type = 3;
  c.ip3 = RED;

  for (;;) {
    c.ip1 = x + _x[idx];
    c.ip2 = y + _y[idx];
    xQueueSend(comQueue, &c, portMAX_DELAY);
    idx++;
    if (idx == 10) {
      idx = 0;
      x = random(0, 240);
      y = random(70, 320);
    }
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
