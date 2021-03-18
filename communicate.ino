#include <SoftwareSerial.h>

/*
 * SerialReceiveMultipleFields sketch
 * This code expects a message in the format: 12,345,678
 * This code requires a newline character to indicate the end of the data
 * Set the serial monitor to send newline characters
 */

SoftwareSerial serOut(10, 11);
SoftwareSerial serIn(8, 9);


const int NUMBER_OF_FIELDS = 2; // how many comma separated fields we expect
int fieldIndex = 0;            // the current field being received
int values[NUMBER_OF_FIELDS];   // array holding values for all the fields



void setup()
{
  serIn.begin(300);
  serOut.begin(300);
  serIn.listen();
  Serial.begin(9600); // Initialize serial port to send and receive at 9600 baud
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  randomSeed(analogRead(0));
}

void loop()
{
  tryOutput();
  checkInput();
}

void checkInput() {
  if(serIn.available())
  {
    digitalWrite(2, HIGH);
    char ch = serIn.read();
    Serial.println(ch);
    if(ch >= '0' && ch <= '9') // is this an ascii digit between 0 and 9?
    {
      // yes, accumulate the value
      values[fieldIndex] = (values[fieldIndex] * 10) + (ch - '0'); 
    }
    else if (ch == ',')  // comma is our separator, so move on to the next field
    {
      if(fieldIndex < NUMBER_OF_FIELDS-1)
        fieldIndex++;   // increment field index
    }
    else
    {
      // any character not a digit or comma ends the acquisition of fields
      // in this example it's the newline character sent by the Serial Monitor
      Serial.print( fieldIndex +1);
      Serial.println(" fields received:");

      if (values[0] == 100) {
        if (values[1] == 1) {
          digitalWrite(3, HIGH);
        } else if (values[1] == 0) {
          digitalWrite(3, LOW);
        }
      }
      
      for(int i=0; i <= fieldIndex; i++)
      {
        Serial.println(values[i]);
        values[i] = 0; // set the values to zero, ready for the next message
      }
      fieldIndex = 0;  // ready to start over
    }
  } else {
    digitalWrite(2, LOW);
  }
}

void tryOutput() {
  int c = random(0, 10000);
  if (c < 10) {
    digitalWrite(4, HIGH);
    int cc = random(0, 100);
    if (cc < 50) {
      ledControl(1);
    } else {
      ledControl(0);
    }
    digitalWrite(4, LOW);
  }
}

void ledControl(int state) {
  if (state == 1) {
    digitalWrite(5, HIGH);
  }
  serOut.print(100);
  serOut.print(",");
  serOut.println(state);
  digitalWrite(5, LOW);
}
