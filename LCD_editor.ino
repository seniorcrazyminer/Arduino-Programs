#include <Elegoo_GFX.h>
#include <Elegoo_TFTLCD.h>
#include <SPI.h>
#include <SD.h>
#include <TouchScreen.h>

File root;

String curDir = "/";

//define LCD_CS A3
//define LCD_CD A2
//define LCD_WR A1
//define LCD_RD A0
//define LCD_RESET A4
#define  BLACK   0x0000
#define BLUE    0x001F
//define RED     0xF800
#define GREEN   0x07E0
//define CYAN    0x07FF
//define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF



Elegoo_TFTLCD tft(A3, A2, A1, A0, A4);



//define YP A3
//define XM A2
//define YM 9
//define XP 8
TouchScreen ts = TouchScreen(8, A3, A2, 9, 300);



void setup() {
  Serial.begin(9600);
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(BLACK);

  pinMode(13, OUTPUT);

  //drawCharBar();

  SD.begin(10);

  root = SD.open(curDir);

  

  tft.setTextColor(WHITE);
  eval("text 0 0 hello;");
  eval("#(a)# = 200;");
  eval("text 0 100 a=;");
  eval("text 30 100 $(a)$;");
  //eval("text 0 20 $(new.txt)$;");
  //tft.println("AVRTerminal v2.0");

  //drawCline();
}

int cx = 0;
int cy = 0;

String com = "";

void loop() {
  char c = getCurButton();
  if (c != -1) {
    tft.print(c);
    if (c == '\n') {
      cli.parse(com);
      com = "";
      drawCline();
    } else {
      com = com + String(c);
    }
  }
}

char characters[] = "abcdefghijklmnopqrstuvwxyz0123456789 ,._=<>\"\'()/;!@#$%^&*~:\n";

int yoff = 95;

void drawCharBar() {
  tft.setTextColor(BLACK);
  for (int x = 0; x < 12; x++) {
    for (int y = 0; y < 5; y++) {
      tft.fillRect(20 * x + 3, 20 * y + (320 - yoff), 15, 15, BLUE);
      tft.setCursor(20 * x + 9, 20 * y + (320 - yoff) + 3);
      tft.print(characters[12 * y + x]);
    }
  }
}


//define TS_MINX 100
//define TS_MINY 120
//define TS_MAXX 920
//define TS_MAXY 940
//define MINPRESSURE 10
//define MAXPRESSURE 1000

char getCurButton() {
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);

  if (p.z > 10 && p.z < 1000) {
    p.x = map(p.x, 100, 920, 0, 240);
    p.y = map(p.y, 120, 940, 320, 0);

    int color;

    char cur = -1;

    int cx = 0;
    int cy = 0;

    for (int x = 0; x < 12; x++) {
      for (int y = 0; y < 5; y++) {
        color = BLUE;
        if (p.x > 20 * x + 3 && p.x < 20 * x + 3 + 15 && p.y > 20 * y + (320 - (yoff - 15)) && p.y < 20 * y + (320 - (yoff - 15)) + 15) {
          cx = x;
          cy = y;
          tft.drawRect(20 * x + 2, 20 * y + (320 - yoff) - 1, 17, 17, YELLOW);
          delay(250);
          tft.drawRect(20 * x + 2, 20 * y + (320 - yoff) - 1, 17, 17, BLACK);
          return characters[12 * cy + cx];
        }
      }
    }
  }
  return -1;
}

void drawCline() {
  tft.setTextColor(GREEN);
  tft.print("\navr");
  tft.setTextColor(BLUE);
  tft.print(":~");
  tft.print(curDir);
  tft.print(" $ "); 
  tft.setTextColor(WHITE);
}

void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      tft.print('\t');
    }
    tft.print(entry.name());
    if (entry.isDirectory()) {
      tft.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      tft.print("\t\t");
      tft.println(entry.size(), DEC);
    }
    entry.close();
  }
}

String eval(String com) {

  String result = "";

  int i = 0;
  int l = 0;
  String args[4];
  String cur = "";
  String c, nc;

  
  while (i < com.length()) {
    c = com.substring(i, i + 1);
    if (i + 1 < com.length()) {
      nc = com.substring(i + 1, i + 2);
    } else {
      nc = " ";
    }
    if (c == " " || c == ";") {
      args[l] = cur;
      cur = "";
      l++;
      i++;
    } else if (c == "$" && nc == "(") {
      int next = com.indexOf(")$", i);
      args[l] = eval("$" + com.substring(i + 2, next) + ";");
      i = next + 2;
      l++;
    } else if (c == "#" && nc == "(") {
      int next = com.indexOf(")#", i);
      args[l] = eval("#" + com.substring(i + 2, next) + ";");
      i = next + 2;
      l++;
    } else if (c == "@" && nc == "(") {
      int next = com.indexOf(")@", i);
      args[l] = eval(com.substring(i + 2, next) + ";");
      i = next + 2;
      l++;
    } else {
      cur = cur + c;
      i++;
    }
  }

  String instr = args[0];

  if (instr == "text") {
    tft.setCursor(args[1].toInt(), args[2].toInt());
    tft.print(args[3]);
  } else if (instr.substring(0, 1) == "$") {
    String vname = instr.substring(1, instr.length());
    if (args[1] == "==") {
      result = (readf(vname) == args[2]);
    } else {
      result = readf(vname);
    }
  } else if (instr.substring(0, 1) == "#") {
    String vname = instr.substring(1, instr.length());
    if (args[1] == "=") {
      writef(vname, args[2]);
    }
  }

  return result;

}

void writef(String n, String val) {
  File f = SD.open(curDir + n, FILE_WRITE);
  f.seek(0);
  f.print(val);
  f.close();
}

String readf(String n) {
  File f = SD.open(curDir + n, FILE_WRITE);
  String cont;
  while(f.available()) {
    cont = cont + f.read();
  }
  f.close();
  return cont;
}
