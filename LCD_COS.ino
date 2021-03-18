#include <Arduino.h>
#include <virtmem.h>
#include <SdFat.h>
#include <alloc/sd_alloc.h>
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

#define YP A3
#define XM A2
#define YM 9
#define XP 8

#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920

#define MINPRESSURE 10
#define MAXPRESSURE 1000

#define ICONW 5
#define ICONH 5
#define SCALE 10

using namespace virtmem;

const uint32_t poolSize = 1024l * 32l; // the size of the virtual memory pool (in bytes)

SdFat SD;
SDVAlloc valloc(poolSize);

Elegoo_TFTLCD _tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void initialize() {
  _tft.reset();
  _tft.begin(0x9341);
  _tft.setRotation(3);
  _tft.fillScreen(BLACK);
}

void txt(int x, int y, String s, int sz, uint16_t col) {
  _tft.setCursor(x, y);
  _tft.setTextColor(col);
  _tft.setTextSize(sz);
  _tft.print(s);
}



void setup() {
  Serial.begin(9600);
  SD.begin(10, SPI_FULL_SPEED);


  initialize();

  runProg("system/prog/user/new.csl");


  valloc.start();

}

void loop() {}

bool inString(String str, String sub) {
  int i = str.indexOf(sub);
  return !(i == -1);
}

String getNextDelim(String s, String del, int numDone) {
  int idx = 0, nextIdx;
  for (int i = 0; i < numDone; i++) {
    idx = s.indexOf(del, idx + 1);
  }
  if (idx == -1) {
    return "!!!";
  } else {
    nextIdx = s.indexOf(del, idx + 1);
    if (nextIdx == -1) {
      return s.substring(idx + 1, s.length());
    } else {
      return s.substring(idx + (idx != 0), nextIdx);
    }

  }
}

String eval(String s) {
  int cur = 0;
  String body, nam;
  String params[8];
  bool isCom;
  int num = 0, num1 = 0;
  if (inString(s, "(")) {
    isCom = true;
    cur = s.indexOf("(");
    nam = s.substring(0, cur);
    body = s.substring(cur + 1, s.length() - 1);

    //Serial.print(nam);
    //Serial.print("  ");
    //Serial.println(body);
    String item;
    int num1 = 0;

    while (item != "!!!") {
      item = getNextDelim(body, ",", num);
      if (inString(item, "(")) {
        int st = body.indexOf(item);
        int i = 0;
        int numSkips = 0;
        while (body.charAt(st + i) != ')') {
          if (body.charAt(st + i) == ',') numSkips++;
          i++;
        }
        item = body.substring(st, st + i + 1);
        num += numSkips + 1;
      } else {
        num++;
      }
      if (item != "!!!") {
        params[num1] = item;
        num1++;
      }
    }

  } else {
    isCom = false;
    body = s;
  }

  String retrn;

  if (isCom) {
    for (int i = 0; i <= num1; i++) {
      params[i] = eval(params[i]);
    }
    if (nam == "text") {
      txt(params[0].toInt(), params[1].toInt(), params[2], params[3].toInt(), params[4].toInt());
      retrn = params[2];
    } else if (nam == "set") {
      setVal(params[0], params[1]);
      retrn = params[1];
    } else if (nam == "run") {
      runProg(params[0]);
    } else if (nam == "if") {
      bool a;
      if (params[1] == "=") {
        a = params[0] == params[2];
      } else if (params[1] == "<") {
        a = params[0] < params[2];
      } else if (params[1] == ">") {
        a = params[0] > params[2];
      }
      if (a == false) {
         retrn = "%";
      }
    } else if (nam == "jump") {
      retrn = "@" + params[0];
    }
  } else {
    if (body.charAt(0) == '$') {
      //String val;
      //if (inString(body, "+")) {
      //val = String(getVal(body.substring(1, body.indexOf('+'))).toInt() + body.substring(body.indexOf('+') + 1, body.length()).toInt());
      //} else {
      retrn = getVal(body.substring(1, body.length()));
      //}
      //retrn = val;
    } else {
      retrn = body;
    }
  }

  return retrn;

}

String getVal(String nam) {
  VPtr<File, SDVAlloc> f = valloc.alloc<File>();
  *f = SD.open("vars", FILE_READ);
  char cur;
  String line, nline = "-1";
  int lineNum = 0;
  while (f->peek() != -1) {
    line = f->readStringUntil('\n');
    lineNum++;
    if (line.substring(0, line.indexOf(":")) == nam) {
      nline = line.substring(line.indexOf(":") + 1, line.length());
    }
  }
  f->close();
  return nline;
}

void setVal(String nam, String val) {
  VPtr<File, SDVAlloc> f = valloc.alloc<File>();
  *f = SD.open("vars", FILE_WRITE);
  f->println(nam + ":" + val);
  f->close();
}

String getLine(File f, int n) {
  //f.setTimeout(0);
  String line;
  int l = 0;
  if (l == n) {
    line = f.readStringUntil('\n');
    line = line.substring(0, line.length() - 1);
  } else {
    while (l < n) {
      line = f.readStringUntil('\n');
      line = line.substring(0, line.length() - 1);
      l++;
    }
  }
  return line;
}

void runProg(String nam) {
  VPtr<File, SDVAlloc> f = valloc.alloc<File>();
  *f = SD.open(nam, FILE_READ);
  String line, res;
  int ln = 0;
  while (line != "RET") {
    line = getLine(*f, ln);
    Serial.println(line);
    res = eval(line);
    Serial.println(res);
    if (res == "%") ln++;
    ln++;
    if (res.charAt(0) == '@') ln = res.substring(2,res.length()).toInt();
  }
  f->close();
}
