#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

SoftwareSerial ser(10, 11);

Adafruit_SSD1306 display(128, 64, &Wire, 4);

int iLen[32] = {0,2,2,2,2,2,2,2,2,3,3,3,3,3,2,2,1,1,2,2,1,0,1,1,1,1,1,1,2,2,1,1};

int pins[6] = {A0, A1, A2, A3, A6, A7};
byte regs[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
byte mem[64];
byte flags = 0x00;

byte stack[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
int sp = 0;


int pmsize = 256;
int msize = 64;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  ser.begin(80000);

  disp();
  delay(2000);
  clr();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  for (int i = 2; i < 10; i++) {
    pinMode(i, OUTPUT);
  }

}

void pxy(int x, int y, String s) {
  display.setCursor(x, y);
  display.print(s);
}

int okeys[9];
int keys[9];

void getkey() {
  for (int i = 0; i < 6; i++) {
    okeys[i] = keys[i];
    keys[i] = analogRead(pins[i]);
  }
}

bool keypress(int k) {
  if (keys[k] == 0 && okeys[k] == 0) {
    return true;
  } else {
    return false;
  }
}

String mmenu[3] = {"Edit progmem", "Edit mem", "Run"};

void loop() {
  int c = menu("OLED_ASM", mmenu, 3);
  if (c == 0) {
    progmemMenu();
  } else  if (c == 1) {
    memMenu();
  } else {
    runprog();
  }

}

int menu(String title, String items[], int len) {
  clr();
  pxy(0, 0, title);
  pxy(0, 20, ">");

  for (int i = 0; i < len; i++) {
    pxy(10, 10 * (i + 2), items[i]);
  }

  int line = 0;
  bool ext = false;

  while (not(ext)) {
    getkey();
    if (keypress(0) || keypress(1)) {
      display.fillRect(0, 10 * (line + 2), 10, 15, SSD1306_BLACK);
      if (keypress(0)) line = line - 1;
      if (keypress(1)) line = line + 1;
      if (line < 0) line = len - 1;
      if (line >= len) line = 0;
      pxy(0, 10 * (line + 2), ">");
      delay(100);
    } else if (keypress(4) || keypress(5)) {
      ext = true;
    }
    disp();
  }

  delay(100);
  return line;
}



int hexToInt(byte hex, int half) {
  half = not(half);
  for (int i = 0; i < 4; i++) {
    bitWrite(hex, i + (4 * half), 0);
  }
  if (half == 0) {
    for (int i = 0; i < 4; i++) {
      bitWrite(hex, i, bitRead(hex, i + 4));
      bitWrite(hex, i + 4, 0);
    }
  }
  return int(hex);
}

const char hexDigits[16] = "0123456789ABCDEF";

int off = 25;

void pd(int i, int digit) {
  display.fillRect(10 * i + off, 20, 10, 10, SSD1306_BLACK);
  display.setCursor(10 * i + off, 20);
  display.print(hexDigits[digit]);
}
char hexToString(int digit) {
  return hexDigits[digit];
}

void clrcursor(int d) {
  display.fillRect(10 * d + off, 40, 10, 10, SSD1306_BLACK);
}

void drwcursor(int d) {
  display.setCursor(10 * d + off, 40);
  display.print("^");
  disp();
}

void disp() {
  display.display();
}

void clr() {
  display.clearDisplay();
}

byte getLineByte(int l, int b) {
  ser.print("r ");
  ser.println(4*l + b);
  return getResponse();  
}

byte getResponse() {
  bool ext = false;
  byte ret;
  ser.listen();
  while (not(ext)) {
    if (ser.available())
    {
      ret = ser.read();
      ext = true;
    }
  }
  return ret;
}

void setLineByte(int l, int b, byte val) {
    ser.print("w ");
    ser.print(4 * l + b);
    ser.print(",");
    ser.println(char(val));
}

void editline(int l) {
  clr();

  byte line[4];

  for (int i = 0; i < 4; i++) {
    line[i] = getLineByte(l, i);
  }

  int digits[8];

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 2; j++) {
      digits[2 * i + j] = hexToInt(line[i], 1 - j);
      pd(2 * i + j, digits[2 * i + j]);
    }
  }

  int digit = 0;

  disp();

  bool ext = false;

  while (not(ext)) {
    getkey();
    if (keypress(0)) {
      digits[digit]--;
      if (digits[digit] < 0) {
        digits[digit] = 15;
      }
      pd(digit, digits[digit]);
      disp();
      delay(50);
    } else if (keypress(1)) {
      digits[digit]++;
      if (digits[digit] > 15) {
        digits[digit] = 0;
      }
      pd(digit, digits[digit]);
      disp();
      delay(50);
    } else if (keypress(2)) {
      clrcursor(digit);
      digit--;
      if (digit < 0) {
        digit = 7;
      }
      drwcursor(digit);
      delay(50);
    } else if (keypress(3)) {
      clrcursor(digit);
      digit++;
      if (digit > 7) {
        digit = 0;
      }
      drwcursor(digit);
      delay(50);
    } else if (keypress(4) || keypress(5)) {
      ext = true;
      delay(100);
    }

  }
  if (keypress(5)) {
    for (int i = 0; i < 4; i++) {
      byte combine = 16 * (digits[2 * i]) + digits[2 * i + 1];
      setLineByte(l, i, combine);
    }
  }
}

void editmline(int l) {
  clr();

  byte line = mem[l];

  int digits[2];

  for (int j = 0; j < 2; j++) {
    digits[j] = hexToInt(line, 1 - j);
    pd(j, digits[j]);
  }

  int digit = 0;

  disp();

  bool ext = false;

  while (not(ext)) {
    getkey();
    if (keypress(0)) {
      digits[digit]--;
      if (digits[digit] < 0) {
        digits[digit] = 15;
      }
      pd(digit, digits[digit]);
      disp();
      delay(50);
    } else if (keypress(1)) {
      digits[digit]++;
      if (digits[digit] > 15) {
        digits[digit] = 0;
      }
      pd(digit, digits[digit]);
      disp();
      delay(50);
    } else if (keypress(2)) {
      clrcursor(digit);
      digit--;
      if (digit < 0) {
        digit = 1;
      }
      drwcursor(digit);
      delay(50);
    } else if (keypress(3)) {
      clrcursor(digit);
      digit++;
      if (digit > 1) {
        digit = 0;
      }
      drwcursor(digit);
      delay(50);
    } else if (keypress(4) || keypress(5)) {
      ext = true;
      delay(100);
    }

  }
  if (keypress(5)) {
    byte combine = 16 * (digits[0]) + digits[1];
    mem[l] = combine;
  }
}

long numInstr = 0;
bool debug = false;

void runprog() {
  clr();
  numInstr = 0;
  int i = 0;
  bool r = true;

  byte instr[4];

  long start = micros();

  while (r) {
    instr[0] = getLineByte(i, 0);
    for (int j = 1; j < 4; j++) {
      if (j <= iLen[instr[0]]) {
        instr[j] = getLineByte(i, j); 
      } else {
        instr[j] = 0x00;
      }
    }
    i = instruction(instr, i);
    getkey();
    if (keypress(4) || i < 0) {
      r = false;
    }
  }
  clr();
  long dur = micros() - start;
  display.setCursor(0, 0);
  display.println("# Instructions:");
  display.println(numInstr);
  display.println("\nTime");
  display.print(dur);
  display.println(" micros");
  disp();
  delay(5000);
}

int instruction(byte i[4], int idx) {
  numInstr++;
  byte o = i[0];
  byte p[3] = {i[1], i[2], i[3]};

  if (o == 0x01) {
    mem[p[0]] = p[1];
  } else if (o == 0x02) {
    mem[p[0]] = regs[p[1]];
  } else if (o == 0x03) {
    mem[regs[p[0]]] = p[1];
  } else if (o == 0x04) {
    mem[regs[p[0]]] = regs[p[1]];
  } else if (o == 0x05) {
    regs[p[0]] = mem[p[1]];
  } else if (o == 0x06) {
    regs[p[0]] = mem[regs[p[1]]];
  } else if (o == 0x07) {
    regs[p[0]] = p[1];
  } else if (o == 0x08) {
    regs[p[0]] = regs[p[1]];
  } else if (o == 0x09) {
    regs[p[2]] = regs[p[0]] + p[1];
  } else if (o == 0x0A) {
    regs[p[2]] = regs[p[0]] + regs[p[1]];
  } else if (o == 0x0B) {
    regs[p[2]] = regs[p[0]] - p[1];
  } else if (o == 0x0C) {
    regs[p[2]] = p[0] - regs[p[1]];
  } else if (o == 0x0D) {
    regs[p[2]] = regs[p[0]] - regs[p[1]];
  } else if (o == 0x0E) {
    compare(regs[p[0]], p[1]);
  } else if (o == 0x0F) {
    compare(regs[p[0]], regs[p[1]]);
  } else if (o == 0x10) {
    idx = p[0];
    idx--;
  } else if (o == 0x11) {
    idx = regs[p[0]];
    idx--;
  } else if (o == 0x12) {
    if (bitRead(flags, p[1])) {
      idx = p[0];
      idx--;
    }
  } else if (o == 0x13) {
    if (bitRead(flags, p[1])) {
      idx = regs[p[0]];
      idx--;
    }
  } else if (o == 0x14) {
    byte out = regs[p[0]];
    for (int i = 0; i < 8; i++) {
      digitalWrite(i + 2, bitRead(out, i));
    }
  } else if (o == 0x15) {
    idx = -2;
  } else if (o == 0x16) {
    regs[p[0]]++;
  } else if (o == 0x17) {
    regs[p[0]]--;
  } else if (o == 0x18) {
    regs[p[0]] = regs[p[0]] << 1;
  } else if (o == 0x19) {
    regs[p[0]] = regs[p[0]] >> 1;
  } else if (o == 0x1A) {
    idx = idx + (p[0] - 128);
    idx--;
  } else if (o == 0x1B) {
    idx = idx +  (regs[p[0]] - 128);
    idx--;
  } else if (o == 0x1C) {
    if (bitRead(flags, p[1])) {
      idx = idx + (p[0] - 128);
      idx--;
    }
  } else if (o == 0x1D) {
    if (bitRead(flags, p[1])) {
      idx = idx + (regs[p[0]] - 128);
      idx--;
    }
  } else if (o == 0x1E) {
    stack[sp] = regs[p[0]];
    sp++;
  } else if (o == 0x1F) {
    sp--;
    regs[p[0]] = stack[sp];
  } else {
    numInstr--;
  }
  idx++;
  if (idx >= pmsize) {
    idx = 0;
  }
  return idx;
}

void compare(byte a, byte b) {
  flags = 0x00;
  if (a == b) {
    bitWrite(flags, 0, 1);
  } else if (a > b) {
    bitWrite(flags, 1, 1);
  } else if (a < b) {
    bitWrite(flags, 2, 1);
  }
}

void progmemMenu() {
  clr();
  pxy(0, 0, ">");

  int mx = 0;

  mx = 6;

  for (int i = 0; i < mx; i++) {
    pwordToString(i, i);
  }

  int dl = 0;
  int sl = 0;

  bool ext = false;

  while (not(ext)) {
    getkey();
    if (keypress(0) || keypress(1)) {
      display.fillRect(0, 10 * (sl), 10, 7, SSD1306_BLACK);
      if (keypress(0)) sl = sl - 1;
      if (keypress(1)) sl = sl + 1;
      if (sl < 0) {
        sl = 0;
        if (dl > 0) {
          dl--;
        }
      }
      if (sl >= mx) {
        sl = mx - 1;
        if (dl < pmsize - 4) {
          dl++;
        }
      }
      for (int i = 0; i < mx; i++) {
        display.fillRect(10, 10 * (i), 118, 7, SSD1306_BLACK);
        pwordToString(dl + i, i);
      }
      pxy(0, 10 * (sl), ">");
      delay(100);
    } else if (keypress(5)) {
      delay(100);
      editline(dl + sl);
      clr();
      for (int i = 0; i < mx; i++) {
        display.fillRect(10, 10 * (i), 118, 7, SSD1306_BLACK);
        pwordToString(dl + i, i);
      }
      pxy(0, 10 * (sl), ">");
      delay(100);
    } else if (keypress(4)) {
      ext = true;
    }
    disp();
  }
}

void memMenu() {
  clr();
  pxy(0, 0, ">");

  int mx = 0;

  mx = 6;

  for (int i = 0; i < mx; i++) {
    wordToString(i, i);
  }

  int dl = 0;
  int sl = 0;

  bool ext = false;

  while (not(ext)) {
    getkey();
    if (keypress(0) || keypress(1)) {
      display.fillRect(0, 10 * (sl), 10, 7, SSD1306_BLACK);
      if (keypress(0)) sl = sl - 1;
      if (keypress(1)) sl = sl + 1;
      if (sl < 0) {
        sl = 0;
        if (dl > 0) {
          dl--;
        }
      }
      if (sl >= mx) {
        sl = mx - 1;
        if (dl < msize - 4) {
          dl++;
        }
      }
      for (int i = 0; i < mx; i++) {
        display.fillRect(10, 10 * (i), 118, 7, SSD1306_BLACK);
        wordToString(dl + i, i);
      }
      pxy(0, 10 * (sl), ">");
      delay(100);
    } else if (keypress(5)) {
      delay(100);
      editmline(dl + sl);
      clr();
      for (int i = 0; i < mx; i++) {
        display.fillRect(10, 10 * (i), 118, 7, SSD1306_BLACK);
        wordToString(dl + i, i);
      }
      pxy(0, 10 * (sl), ">");
      delay(100);
    } else if (keypress(4)) {
      ext = true;
    }
    disp();
  }
}

void pwordToString(int idx, int l) {
  byte line[4];
  for (int i = 0; i < 4; i++) {
    line[i] = getLineByte(idx, i);
  }
  int digits[8];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 2; j++) {
      display.fillRect(10 * (2 * i + j) + 10, 10 * (l), 10, 10, SSD1306_BLACK);
      display.setCursor(10 * (2 * i + j) + 10, 10 * (l));
      display.print(hexDigits[hexToInt(line[i], 1 - j)]);
    }

  }

}

void wordToString(int idx, int l) {
  byte line;
  line = mem[idx];

  int digits[2];
  for (int j = 0; j < 2; j++) {
    display.fillRect(10 * (j) + 10, 10 * (l), 10, 10, SSD1306_BLACK);
    display.setCursor(10 * (j) + 10, 10 * (l));
    display.print(hexDigits[hexToInt(line, 1 - j)]);
  }
}
