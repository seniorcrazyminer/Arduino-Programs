#include <Elegoo_GFX.h>
#include <Elegoo_TFTLCD.h>
#include <TouchScreen.h>

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


#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//Touch For New ILI9341 TP
#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);


Elegoo_GFX_Button buttons[9];


char buttonlabels[9][5] = {"+", "-", "^", ">", "v", "<", "draw", "scl+", "scl-"};
uint16_t buttoncolors[9] = {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK};


int scale = 1;

struct i {
  float x, y;
  i operator+(i r) {
    return i(x + r.x, y + r.y);
  }
  i operator*(i r) {
    return i((x * r.x) - (y * r.y), (x * r.y + y * r.x));
  }
  i() {}
  i(float a, float b) {
    x = a;
    y = b;
  }
  float operator~() {
    return sqrt((x * x) + (y * y));
  }
  String tostring() {
    return String(x) + " + " + String(y) + "i";
  }
};

void setup() {
  Serial.begin(9600);
  tft.reset();

  uint16_t identifier = tft.readID();
  identifier = 0x9341;

  tft.begin(identifier);
  tft.setRotation(3);
  tft.fillScreen(BLACK);

  for (uint8_t row = 0; row < 9; row++) {
    buttons[row].initButton(&tft, 280,
                            20 + row * (25), // x, y, w, h, outline, fill, text
                            40, 20, WHITE, buttoncolors[row], WHITE,
                            buttonlabels[row], 1);
    buttons[row].drawButton();
  }

  drawFullScreen();

}

float zoom = 1.0;
float sx = 0;
float sy = 0;

void loop() {
  tick();
}

float dc = 1;

void tick() {
  int p = getPress();
  bool redraw = false;
  if (p == 0) {
    zoom = zoom * 1.5;
  } else if (p == 1) {
    zoom = zoom / 1.5;
  } else if (p == 2) {
    sy = sy - (dc*scale)/zoom;
  } else if (p == 3) {
    sx = sx + (dc*scale)/zoom;
  } else if (p == 4) {
    sy = sy + (dc*scale)/zoom;
  } else if (p == 5) {
    sx = sx - (dc*scale)/zoom;
  } else if (p == 6) {
    drawFullScreen();
  } else if (p == 7) {
    scale++;
  } else if (p == 8) {
    scale--;
  }
}

bool tickDuring() {
  int p = getPress();
  if (p == 0) {
    zoom = zoom * 1.5;
  } else if (p == 1) {
    zoom = zoom / 1.5;
  } else if (p == 2) {
    sy = sy - (dc*scale)/zoom;
  } else if (p == 3) {
    sx = sx + (dc*scale)/zoom;
  } else if (p == 4) {
    sy = sy + (dc*scale)/zoom;
  } else if (p == 5) {
    sx = sx - (dc*scale)/zoom;
  } else if (p == 7) {
    scale++;
  } else if (p == 8) {
    scale--;
  }
  if (p != -1) {
    return true;
  } else {
    return false;
  }
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

int getPress() {
  int bb = -1;
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    p.x = map(p.x, TS_MINY, TS_MAXY, tft.width(), 0);
    p.y = (map(p.y, TS_MINX, TS_MAXX, tft.height(), 0));
    if (p.x < 100) {
      p.x += 25;
    }
  }

  

  // go thru all the buttons, checking if they were pressed
  for (uint8_t b = 0; b < 9; b++) {
    if (buttons[b].contains(320 - p.y, 280 - p.x)) {
      buttons[b].press(true);  // tell the button it is pressed
    } else {
      buttons[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // now we can ask the buttons if their state has changed
  for (uint8_t b = 0; b < 9; b++) {
    if (buttons[b].justReleased()) {
      // Serial.print("Released: "); Serial.println(b);
      buttons[b].drawButton();  // draw normal
    }

    if (buttons[b].justPressed()) {
      buttons[b].drawButton(true);  // draw invert!
      bb = b;
      delay(100); // UI debouncing
    }
  }
  return bb;
}

bool drawFullScreen() {
  for (int x = -120; x < 120; x += scale) {
    for (int y = -120; y < 120; y += scale) {
      if (tickDuring()) return true;
      float xx = sx + (x / zoom);
      float yy = sy + (y / zoom);
      if (scale == 1) {
        tft.drawPixel(x + 120, y + 120, WHITE);
      } else {
        tft.fillRect(x + 120, y + 120, scale, scale, WHITE);
      }
      
      int it = 256 - iterate(i(xx, yy));
      if (scale == 1) {
        tft.drawPixel(x + 120, y + 120, tft.color565(it, it, it));
      } else {
        tft.fillRect(x + 120, y + 120, scale, scale, tft.color565(it, it, it));
      }
    }
  }
  return false;
}

int maxIterations = 250;

int iterate(i c) {
  i z = i(0, 0);
  int j = 0;
  while (j < maxIterations && ~z <= 2.0) {
    z = (z * z) + c;
    j++;
  }
  return j;
}
