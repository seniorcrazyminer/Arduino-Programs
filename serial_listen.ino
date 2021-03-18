#include <SoftwareSerial.h>

SoftwareSerial serOut(10, 11);
SoftwareSerial serIn(8, 9);

void setup()
{
  serOut.begin(300);
  serIn.begin(300);
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
}


char message[10] = "";
int cur = 0;

void loop()
{
  /*if (random(0, 5000) == 0) {
    if (random(0, 100) < 50) {
      send("ON");
    } else {
      send("OFF");
    }
  }*/
  check();
}

void understand(char msg[]) {
  Serial.println(msg);
  Serial.println("-----");
  Serial.println(msg[0]);
  Serial.println(msg[1]);
  Serial.println(msg[2]);
  if (msg[0] == "O" && msg[1] == "N") {
    digitalWrite(3, HIGH);
  }
  if (msg[0] == "O" && msg[1] == "F" && msg[2] == "F") {
    digitalWrite(3, LOW);
  }
}

void send(char data[]) {
  char e = 100;
  put(data);
  put(e);
}

void put(char data[]) {
  Serial.print("OUT");
  Serial.println(data);
  serOut.write(data);
}

void check() {
  char msg[] = "";
  int c = 0;
  char chr;
  bool r = true;
  if (serIn.available() > 0) {
    while (r) {
      if (chr != "@" && serIn.available() > 0) {
        digitalWrite(2, HIGH);
        chr = get();
        msg[c] = chr;
        c = c + 1;
      } else {
        digitalWrite(2, LOW);
        r = false;
      }
    }
    understand(msg);
  }
}

char get() {
  char c = serIn.read();
  Serial.println(c);
  return (c);
}
