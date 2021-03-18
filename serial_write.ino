#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11);

void setup()
{
  mySerial.begin(300);
  Serial.begin(9600);
}

void loop()
{
  mySerial.write(45);
  int bytesSent = mySerial.write("hello");
  Serial.println(bytesSent);
}
