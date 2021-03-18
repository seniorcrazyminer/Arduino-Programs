#include <SoftwareSerial.h>
#include <EEPROM.h>

SoftwareSerial ser(10, 11);

const int NUMBER_OF_FIELDS = 25;
int fieldIndex1 = 0;
char values1[25];

const int mSize = 1024;
byte mem[1024];

void setup()
{
  ser.begin(80000);
  for (int i = 0; i < mSize; i++) {
    mem[i] = 0x00;
  }
  pinMode(LED_BUILTIN, OUTPUT);
  ser.listen();
}

void loop()
{
  
  if (ser.available())
  {
    char ch = ser.read();
    if (ch == '\n')
    {
      char val[fieldIndex1];
      for (int i = 0; i <= fieldIndex1; i++)
      {
        val[i] = values1[i];
        values1[i] = 0;
      }
      action(String(val));
      ser.listen();
      fieldIndex1 = 0;
    } else {
      values1[fieldIndex1] = ch;
      if (fieldIndex1 < NUMBER_OF_FIELDS - 1)
        fieldIndex1++;
    }
  }
}

void action(String dat) {
  digitalWrite(LED_BUILTIN, HIGH);
  String ssss = dat.substring(0, 4);  
  String s = dat.substring(0, 1);  
  int com = dat.indexOf(",");
  if (s == "w") {
    if (com != -1) {
      int a = dat.substring(2, com).toInt();
      byte b = dat.substring(com + 1, dat.length()).charAt(0);
      if (a < mSize) {
        mem[a] = b;
      }
    }
  } else if (s == "r") {
    int b = dat.substring(2, dat.length()).toInt();
    if (b >= 0 && b < mSize) {
      ser.print(char(mem[b]));
    }
  }
  digitalWrite(LED_BUILTIN, LOW);
}
