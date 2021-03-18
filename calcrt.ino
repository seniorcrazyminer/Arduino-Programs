#include <Elegoo_GFX.h>
#include <Elegoo_TFTLCD.h>
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);



int g = 1;
int col[6] = {12, 24, 23, 22, 21, 20};
int ccol[15] = {BLUE, RED, BLACK, MAGENTA, GREEN, 0x0000, 0x0000, 0x0000, CYAN, YELLOW, WHITE, tft.color565(100, 100, 100), tft.color565(80, 80, 80), tft.color565(50, 50, 50), tft.color565(30, 30, 30)};

float dest[3];

void setup() {
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));

#ifdef USE_Elegoo_SHIELD_PINOUT
  Serial.println(F("Using Elegoo 2.8\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Elegoo 2.8\" TFT Breakout Board Pinout"));
#endif

  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();

  uint16_t identifier = tft.readID();
identifier=0x9341;
  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    return;
  }

  tft.begin(identifier);
  tft.setRotation(3);
  tft.fillScreen(BLACK);
  

}

void loop() {
  render();

}

void render() {
  for (float y = 0; y < 240; y += 2 * g) {
    
    for (float x = 0; x < 320; x += 2 * g) {
      float b = (120 - y) / 240;
      float a = (x - 160) / 240;

      float e = (a * a) + (b * b) + 1;
      float f = (-2 * b) - 12;
      float h = 34.75;
      float d = (f * f) - 4 * (e * h);

      float t = -1 / b;

      float u = a * t;
      float v = t;

      int c = remainder(abs(intf(u) + intf(v)), 2);

      int lc = 12;
      int mc = 12;

      c = 12 - (t > 0)*(c + 1);

      if (d >= 0) {
        t = (-f - sqrt(d)) / (2 * e);
        float i = a * t;
        float j = (b * t) - 1;
        float k = 6 - t;
        float m = j / 1.5;

        if (m > 0) {
          m = intf(10 * m);
          lc = col[intf(m / 2)];
          mc = col[intf((m / 2) + .5)];
        } else {
          mc = 12;
        }

        float L3[] = {i / 1.5, j / 1.5, k / 1.5};
        float L2[] = {a, b, 1};

        mult(L3, L2);
         
        float s = 2 * lsum(dest);

        smult(s, L3);

        sub(dest, L2);
        float p = dest[0];
        float q = dest[1];
        float r = dest[2];

        t = (j-2)/q;

        if (t > 0) {
          u = (p*t) + i;
          v = (r*t) + 6 - k;
          c = 11 - remainder(abs(intf(u) + intf(v)), 2);
        } else {
          c = 12;
        }

      } else {
        if (t > 0) {
          e = 1;
          f = 4;
          h = (u*u) + (v*v) - (12*v) + 37.75;
          d = (f*f) - 4*(e*h);

          lc= 12*(d>=0) + c*(d<0);
          mc = lc;
        }

      }
      pixelOn(x, y, c);
      pixelOn(x, y+1, lc);
      pixelOn(x+1, y, mc);
      pixelOn(x+1, y+1, c);
    }
  }
}

void pixelOn(int x, int y, int c) {
  tft.drawPixel(x, y, ccol[c - 10]);
}

int intf(float a) {
  return floor(a);
}

int remainder(int a, int b) {
  return a % b;
}

float sum(float a[]) {
  float s = 0;
  for (int i = 0; i < 3; i++) {
    s = s + a[i];
  }
  return s;
}

void mult(float a[], float b[]) {
  for (int i = 0; i < 3; i++) {
    dest[i] = a[i] * b[i];
  }
}

void smult(float a, float b[]) {
  for (int i = 0; i < 3; i++) {
    dest[i] = a * b[i];
  }
}

void sub(float a[], float b[]) {
  for (int i = 0; i < 3; i++) {
    dest[i] = a[i] - b[i];
  }
}

float lsum(float a[]) {
  float s;
  for (int i = 0; i < 3; i++) {
    s += a[i];
  }
  return s;
}
