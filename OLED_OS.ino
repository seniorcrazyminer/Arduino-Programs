#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 4);

int pins[3] = {A1, A2, A3};


void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.display();
  delay(2000);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text

  for (int i = 0; i < 2; i++) {
    pinMode(pins[i], INPUT);
  }

}

int y = 0;

void p(String s) {
  display.print(s);
}

void pxy(int x, int y, String s) {
  display.setCursor(x, y);
  display.print(s);
}

void pl(String s) {
  y = y + 1;
  if (y > 8) {
    y = 0;
    display.clearDisplay();
    display.setCursor(0, 0);
  }
  display.println(s);
}

int keys[3] = {0, 0, 0};

void getkey() {
  for (int i = 0; i <= 2; i++) {
    keys[i] = analogRead(pins[i]);
  }
}

void clearkey() {
  for (int i = 0; i <= 2; i++) {
    keys[i] = 0;
  }
}


void loop() {
  String homei[] = {"File Explorer", "Pins", "...", "..."};
  int l = menu("OLED OS", homei, 4);
  if (l == 0) {
    String files[] = {"1", "2", "3", "4", "Back"};
    int lf = longMenu("File Explorer", files, 5);
  } else if (l == 1) {
    String pinsi[] = {"2", "3", "4", "5", "Back"};
    int lp = longMenu("Pins", pinsi, 5);
    if (lp != 4) {
      String actionsi[] = {"Mode", "State", "Back"};
      int la = menu("Pin " + pinsi[lp], actionsi, 3);
      if (la == 0) {
        String modei[] = {"INPUT", "OUTPUT"};
        int lm = menu("Mode", modei, 2);
        if (lm == 0) {
          pinMode(pinsi[lp].toInt(), INPUT);
        } else {
          pinMode(pinsi[lp].toInt(), OUTPUT);
        }
      } else if (la == 1) {
        String statei[] = {"HIGH", "LOW"};
        int ls = menu("State", statei, 2);
        if (ls == 0) {
          digitalWrite(pinsi[lp].toInt(), HIGH);
        } else {
          digitalWrite(pinsi[lp].toInt(), LOW);
        }
      }
    }

  }
  display.display();
}

int menu(String title, String items[], int len) {
  display.clearDisplay();
  pxy(0, 0, title);
  pxy(0, 20, ">");

  clearkey();

  for (int i = 0; i < len; i++) {
    pxy(10, 10 * (i + 2), items[i]);
  }

  int line = 0;
  bool ext = false;

  while (not(ext)) {
    getkey();
    if (keys[0] == 1023 || keys[1] == 1023) {
      display.fillRect(0, 10 * (line + 2), 10, 15, SSD1306_BLACK);
      if (keys[0] == 1023) line = line - 1;
      if (keys[1] == 1023) line = line + 1;
      if (line < 0) line = len - 1;
      if (line >= len) line = 0;
      pxy(0, 10 * (line + 2), ">");
      delay(100);
    } else if (keys[2] == 1023) {
      ext = true;
    }
    display.display();
  }

  delay(100);
  return line;
}

int longMenu(String title, String items[], int len) {
  display.clearDisplay();
  pxy(0, 0, title);
  pxy(0, 20, ">");

  clearkey();

  int mx = 0;

  if (len > 4) {
    mx = 4;
  } else {
    mx = len;
  }

  for (int i = 0; i < mx; i++) {
    pxy(10, 10 * (i + 2), items[i]);
  }

  int dl = 0;
  int sl = 0;

  bool ext = false;

  while (not(ext)) {
    getkey();
    if (keys[0] == 1023 || keys[1] == 1023) {
      display.fillRect(0, 10 * (sl + 2), 10, 7, SSD1306_BLACK);
      if (keys[0] == 1023) sl = sl - 1;
      if (keys[1] == 1023) sl = sl + 1;
      if (sl < 0) {
        sl = 0;
        if (dl > 0) {
          dl--;
        }
      }
      if (sl >= mx) {
        sl = mx - 1;
        if (dl < len - 4) {
          dl++;
        }
      }
      for (int i = 0; i < mx; i++) {
        display.fillRect(10, 10 * (i + 2), 118, 7, SSD1306_BLACK);
        pxy(10, 10 * (i + 2), items[dl + i]);
      }
      pxy(0, 10 * (sl + 2), ">");
      delay(100);
    } else if (keys[2] == 1023) {
      ext = true;
    }
    display.display();
  }

  delay(100);
  return dl + sl;
}
