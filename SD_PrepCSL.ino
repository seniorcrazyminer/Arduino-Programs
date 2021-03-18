#include <SdFat.h>

SdFat SD;

void setup() {
  SD.begin(10, SPI_FULL_SPEED);
  SD.remove("system/prog/user/new.csl");

  File f = SD.open("system/prog/user/new.csl", FILE_WRITE);
  f.println("text(0,0,hello,1,65000)");
  f.println("text(0,20,world,1,64000)");
  f.println("text(0,40,how,1,63000)");
  f.println("text(0,60,are,1,62000)");
  f.println("text(0,80,you?,1,61000)");
  f.println("if($a,=,-1)");
  f.println("text(50,100,a=-1,1,40000)");
  f.println("text(50,0,hello,1,15000)");
  f.println("text(50,20,world,1,14000)");
  f.println("text(50,40,how,1,13000)");
  f.println("text(50,60,are,1,12000)");
  f.println("text(50,80,you?,1,11000)");
  f.println("RET");
  f.close();

}

void loop() {
  // put your main code here, to run repeatedly:

}
