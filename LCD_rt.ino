#include <Elegoo_GFX.h>
#include <Elegoo_TFTLCD.h>
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
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
typedef int i;
typedef float f;
struct v{
  f x,y,z;
  v operator+(v r){
    return v(x+r.x,y+r.y,z+r.z);
  }
  v operator*(f r){
    return v(x*r,y*r,z*r);
  }
  f operator%(v r){
    return x*r.x+y*r.y+z*r.z;
  }
  v(){}
  v operator^(v r){
    return v(y*r.z-z*r.y,z*r.x-x*r.z,x*r.y-y*r.x);
  }
  v(f a,f b,f c){
    x=a;
    y=b;
    z=c;
  }
  v operator!(){
    return*this*(1/sqrt(*this%*this));
  }
};
i G[]={247570,280596,280600,249748,18578,18577,231184,16,16};
f R(){
  return(f)rand()/RAND_MAX;
}
i T(v o,v d,f&t,v&n){
  t=1e9;
  i m=0;
  f p=-o.z/d.z;
  if(.01<p)t=p,n=v(0,0,1),m=1;
  for(i k=19;k--;)for(i j=9;j--;)if(G[j]&1<<k){
    v p=o+v(-k,0,-j-4);
    f b=p%d,c=p%p-1,q=b*b-c;
    if(q>0){
      f s=-b-sqrt(q);
      if(s<t&&s>.01)t=s,n=!(p+d*t),m=2;
    }
  }
  v pp=o+v(0,10,10);
  f b=pp%d,c=pp%pp-1,q=b*b-c;
  if(q>0){
    f s=-b-sqrt(q);
    if(s<t&&s>.01)t=s,n=!(pp+d*t),m=2;
  }
  return m;
}
v S(v o,v d){
  f t;
  v n;
  i m=T(o,d,t,n);
  if(!m)return v(.7,.6,1)*pow(1-d.z,4);
  v h=o+d*t,l=!(v(9+R(),9+R(),16)+h*-1),r=d+n*(n%d*-2);
  f b=l%n;
  if(b<0||T(h,l,t,n))b=0;
  f p=pow(l%r*(b>0),99);
  if(m&1){
    h=h*.2;
    return((i)(ceil(h.x)+ceil(h.y))&1?v(3,1,1):v(3,3,3))*(b*.2+.1);
  }
  return v(p,p,p)+S(h,r)*.5;
}
void make(){
  tft.fillScreen(WHITE);
  v g=!v(-6,-16,0),a=!(v(0,0,1)^g)*.002,b=!(g^a)*.002,c=(a+b)*-256+g;
  i n=0;
  for(i y=240;y-=8;)for(i x=320;x--;){
    v p(13,13,13);
    for(i r=64;r--;){
      v t=a*(R()-.5)*99+b*(R()-.5)*99;
      p=S(v(17,16,8)+t,!(t*-1+(a*(R()+x)+b*(y+R())+c)*16))*3.5+p;
    }
    tft.drawPixel(x, y, tft.color565((i)p.x, (i)p.y, (i)p.z));
  }
}





int g = 1;
int col[6] = {12, 24, 23, 22, 21, 20};
int ccol[15] = {BLUE, RED, BLACK, MAGENTA, GREEN, 0x0000, 0x0000, 0x0000, CYAN, YELLOW, WHITE, tft.color565(100, 100, 100), tft.color565(80, 80, 80), tft.color565(50, 50, 50), tft.color565(30, 30, 30)};

float dest[3];

void setup() {
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));

#ifdef USE_Elegoo_SHIELD_PINOUT
  Serial.println(F("Using Elegoo 2.8\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Elegoo 2.8\" TFT Breakout Board Pinout"));
#endif

  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();

  uint16_t identifier = tft.readID();
identifier=0x9341;
  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    return;
  }

  tft.begin(identifier);
  tft.setRotation(3);
  tft.fillScreen(BLACK);
  

}

void loop() {
  make();
}
