#include <PJONSoftwareBitBang.h>

int buses[3] = {2, 3, 1};

PJONSoftwareBitBang bus(buses[0]);

int leds[4] = {2, 4, 6, 8};
String page1 = "000000010011011111110000";
String page2 = "000010001100111011110000";
String page3 = "000010000100001000010000";
String page4 = "000000010010010010000000";
String page5 = "000010010110100101100000";
String page6 = "111100001111000011110000";
String page7 = "000010000100100001000000";
String page8 = "000000010010000100100000";
int numPages = 8;
String pages[8] = {page1, page2, page3, page4, page5, page6, page7, page8};

bool reading;
int index = 0;
int max = 0;
String cur;

bool requested = false;
String dat;
int sid;
int len;


//char page[26] = "<p>This is a paragraph</p>";

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  for (int i = 0; i < length; i++ ) {
    Serial.print((char)payload[i]);
  }
  Serial.println("");
  static bool led_on = false;
  digitalWrite(LED_BUILTIN, led_on ? HIGH : LOW);
  led_on = !led_on;
  char c[length];
  for (int i = 0; i < length; i++) {
    c[i] = (char)payload[i];
  }
  String cmd = String(c);
  if (cmd.substring(0, 3) == "GET") {
    int num = cmd.substring(4, cmd.length()).toInt();
    dat = "CBP " + pages[num];
    requested = true;
    sid = packet_info.tx.id;
    len = dat.length() + 1;
    Serial.print("sent page ");
    Serial.print(num);
    Serial.print(" to device ");
    Serial.println(sid);
  } else if (cmd.substring(0, 3) == "CBP") {
    cur = cmd.substring(4, cmd.length());
    reading = true;
    index = 0;
    max = cur.length() / 4;
  }
};

void test() {
  cur = page3;
  reading = true;
  index = 0;
  max = cur.length() / 4;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(11, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off

  for (int i = 0; i < 4; i++) {
    pinMode(leds[i], OUTPUT);
  }

  bus.strategy.set_pin(12);
  bus.set_receiver(receiver_function);
  bus.begin();
  Serial.begin(9600);
};

int randDest() {
  int dest;
  if (random(0, 100) < 50) {
    dest = buses[1];
  } else {
    dest = buses[2];
  }
  return dest;
}

void loop() {

  if (reading) {
    read();
  } else {
    bus.receive(1000);
    if (random(0, 100) < 10) {
      String a = "GET ";
      a = a + String(random(0,numPages));
      char arr[a.length()];
      a.toCharArray(arr, a.length() + 1);
      bus.send_packet(randDest(), arr, 5);
    }
  }
  if (requested) {
    char arr[len];
    dat.toCharArray(arr, len);
    Serial.println(bus.send_packet_blocking(sid, arr, len));
    requested = false;
  }
  bus.update();
};

void read() {
  String line = cur.substring(4 * index, 4 * index + 5);
  char c[5];
  line.toCharArray(c, 5);
  for (int i = 0; i < 4; i++) {
    if (c[i] == '0') {
      digitalWrite(leds[i], LOW);
    } else {
      digitalWrite(leds[i], HIGH);
    }
  }
  index++;
  if (index > max) {
    reading = false;
    index = 0;
    max = 0;
  }
  delay(100);
}
