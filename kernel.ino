#include <SoftwareSerial.h>
#include <EEPROM.h>

SoftwareSerial serIn(8, 9);
SoftwareSerial serOut(10, 11);

const int NUMBER_OF_FIELDS = 25;
int fieldIndex = 0;
int fieldIndex1 = 0;
char values[25];
char values1[25];

const int mSize = 256;
char mem[256];

String z = String("$");

String prog[20];

int id() {
  return EEPROM.read(0);
}

void setup()
{
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  serIn.begin(9600);
  serOut.begin(9600);
  serIn.listen();
  Serial.begin(9600);
  serOut.print("id ");
  serOut.println(id());
  for (int i = 0; i < mSize; i++) {
    mem[i] = 0;
  }
  Serial.println("AVRTerminal v2.0");
}

void loop()
{
  if (Serial.available())
  {
    digitalWrite(2, HIGH);
    char ch = Serial.read();
    digitalWrite(2, LOW);
    if (ch == '\n')
    {
      char val[fieldIndex];
      for (int i = 0; i <= fieldIndex; i++)
      {
        val[i] = values[i];
        values[i] = 0;
      }
      action(String(val));
      fieldIndex = 0;
    } else {
      values[fieldIndex] = ch;
      if (fieldIndex < NUMBER_OF_FIELDS - 1)
        fieldIndex++;
    }
  }

  if (serIn.available())
  {
    digitalWrite(2, HIGH);
    char ch = serIn.read();
    digitalWrite(2, LOW);
    if (ch == '\n')
    {
      char val[fieldIndex1];
      for (int i = 0; i <= fieldIndex1; i++)
      {
        val[i] = values1[i];
        values1[i] = 0;
      }
      action(String(val));
      fieldIndex1 = 0;
    } else {
      values1[fieldIndex1] = ch;
      if (fieldIndex1 < NUMBER_OF_FIELDS - 1)
        fieldIndex1++;
    }
  }
}

void action(String dat) {
  String ss = dat.substring(0, 2);
  String sss = dat.substring(0, 3);
  String ssss = dat.substring(0, 4);
  String sssss = dat.substring(0, 5);
  String ssssss = dat.substring(0, 6);
  int com = dat.indexOf(",");
  int eq = dat.indexOf("=");

  if (sss == "add") {
    if (com != -1) {
      float a = dat.substring(4, com).toFloat();
      float b = dat.substring(com + 1, dat.length()).toFloat();
      Serial.println(a+b);
    }
  } else if (sss == "sub") {
    if (com != -1) {
      float a = dat.substring(4, com).toFloat();
      float b = dat.substring(com + 1, dat.length()).toFloat();
      Serial.println(a-b);
    }
  } else if (sss == "mlt") {
    if (com != -1) {
      float a = dat.substring(4, com).toFloat();
      float b = dat.substring(com + 1, dat.length()).toFloat();
      Serial.println(a*b);
    }
  } else if (sss == "dvd") {
    if (com != -1) {
      float a = dat.substring(4, com).toFloat();
      float b = dat.substring(com + 1, dat.length()).toFloat();
      Serial.println(a/b);
    }
  } else if (ssss == "poke") {
    if (com != -1) {
      int a = dat.substring(5, com).toInt();
      char b = dat.substring(com + 1, dat.length()).charAt(0);
      mem[a] = b;
    }
  } else if (ssss == "peek") {
    int b = dat.substring(5, dat.length()).toInt();
    Serial.println(mem[b]);
  } else if (ssss == "dump") {
    Serial.println("START");
    for (int a = 0; a < (mSize / 32); a++) {
      for (int b = 0; b < 32; b++) {
        if (mem[(32 * a) + b] == 0) {
          Serial.print(" ");
        } else {
          Serial.print(mem[(32 * a) + b]);
        }
      }
      Serial.print("\n");
    }
    Serial.println("END");
  } else if (sss == "let") {
    if (eq != -1) {
      String a = dat.substring(4, eq);
      int b = dat.substring(eq + 1, dat.length()).toInt();
      newVar(a, b);
    }
  } else if (sss == "set") {
    if (com != -1) {
      String a = dat.substring(4, com);
      int b = dat.substring(com + 1, dat.length()).toInt();
      setVar(a, b);
    }
  } else if (ssssss == "delvar") {
    String a = dat.substring(7, dat.length());
    delVar(a);
  } else if (sss == "get") {
    String a = dat.substring(4, dat.length());
    Serial.println(getVar(a));
  } else if (sssss == "reset") {
    setup();
  } else if (ssss == "uart") {
    String a = dat.substring(5, dat.length());
    if (a.substring(0, 2) == "-t") {
      String e = a.substring(3, a.length());
      if (e.substring(0, 2) == "-v") {
        String ee = e.substring(3, e.length());
        digitalWrite(4, HIGH);
        serOut.print("let ");
        serOut.print(ee);
        serOut.print("=");
        serOut.println(getVar(ee));
        digitalWrite(4, LOW);
      } else {
        digitalWrite(4, HIGH);
        serOut.println(e);
        digitalWrite(4, LOW);
      }
    } else if (a.substring(0, 2) == "-d") {
      Serial.println("transmitting...");
      for (int i = 0; i < (mSize / 32); i++) {
        for (int j = 0; j < 32; j++) {
          digitalWrite(4, HIGH);
          serOut.print("poke ");
          serOut.print((32 * i) + j);
          serOut.print(",");
          serOut.println(mem[(32 * i) + j]);
          digitalWrite(4, LOW);
          Serial.print(">");
        }
        Serial.print("\n");
      }
    }
  } else if (ss == "id") {
    int a = dat.substring(3, dat.length()).toInt();
    Serial.print("Connection request by board ");
    Serial.println(a);
    digitalWrite(4, HIGH);
    serOut.print("acc ");
    serOut.println(id());
    digitalWrite(4, LOW);
  } else if (sss == "acc") {
    int a = dat.substring(4, dat.length()).toInt();
    Serial.print("Now connected to board ");
    Serial.println(a);
  } else if (ssss == "prog") {
    String a = dat.substring(5, dat.length());
    Serial.println(a);
    int col = a.indexOf(":");
    int line = a.substring(0, col).toInt();
    prog[line] = a.substring(col + 2, a.length());
  } else if (sss == "run") {
    /*
    String a = dat.substring(4, dat.length());
    if (a.substring(0, 2) == "-l") {
      int e = a.substring(3, a.length()).toInt();
      for (int l = 0; l < e; l++) {
        for (int i = 0; i < 20; i++) {
          Serial.println(prog[i]);
          action(prog[i]);
        }
      }
    } else if (a.substring(0, 2) == "-f") {
      while (true) {
        for (int i = 0; i < 20; i++) {
          action(prog[i]);
        }
      }
    }
    */

    for (int i = 0; i < 20; i++) {
      action(prog[i]);
    }
  } else {
    if (dat.length() != 0) {
      Serial.print("cnf: ");
      Serial.println(dat);
    }
  }
}

void setVar(String nam, int val) {
  newVar(nam, val);
}

int findSpace(int siz) {
  int i = 0;
  int j = 0;
  bool found = false;
  while (i < (mSize - siz)) {
    found = true;
    j = 0;
    while (j < siz) {
      if (mem[i + j] != 0) {
        found = false;
      }
      j++;
    }
    if (found) {
      break;
    }
    i++;
  }
  if (found) {
    return i;
  } else {
    return -1;
  }
}

int getVar(String nam) {
  String mm = String(mem);
  String nam1 = "$" + nam + "$";
  int st = mm.indexOf(nam1) + nam1.length();
  int nd = mm.indexOf("$", st);
  return mm.substring(st, nd).toInt();
}

void delVar(String nam) {
  String mm = String(mem);
  String nam1 = "$" + nam + "$";
  int st = mm.indexOf(nam1);
  if (st != -1) {
    st += nam1.length();
    int nd = mm.indexOf("$", st);
    for (int i = st - nam1.length(); i <= nd; i++) {
      mem[i] = 0;
    }
  }
}

void newVar(String nam, int val) {
  delVar(nam);
  String var = String(z + nam + z + String(val) + z);
  int slot = findSpace(var.length());
  Serial.println(var);
  if (slot != -1) {
    int l = var.length();
    char newv[l];
    var.toCharArray(newv, l + 1);
    for (int i = slot; i < slot + l; i++) {
      mem[i] = newv[i - slot];
    }
  }
}
