#include <SoftwareSerial.h>

/*
 * SerialFormatting
 * Print values in various formats to the serial port
*/

SoftwareSerial serOut(10, 11);


void setup()
{
  serOut.begin(300);
}

void loop()
{
  serOut.println("reset");
  delay(1000);
  serOut.println("let a=1");
  delay(1000);
  serOut.println("let b=100");
  delay(1000);
  serOut.println("dump");
  delay(1000);
  serOut.println("musb");
  delay(1000);
}

void snd(int data) {
  serOut.print(data);
}

void sep() {
  serOut.print(",");
}

void tail() {
  serOut.println("");
}
