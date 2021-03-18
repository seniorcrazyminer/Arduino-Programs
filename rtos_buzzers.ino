#include <Arduino_FreeRTOS.h>

float f_1;
float f_2;

void Buzzer1(void* pvParameters);
void Buzzer2(void* pvParameters);

// the setup function runs once when you press reset or power the board
void setup() {
  f_1 = note("Bb", 3);
  f_2 = note("D", 3);
  xTaskCreate(Buzzer1, "Buzzer One", 128, NULL, 0, NULL);
  xTaskCreate(Buzzer2, "Buzzer Two", 128, NULL, 0, NULL);
}

void loop()
{
  // Empty. Things are done in Tasks.
}

void Buzzer1(void *pvParameters)
{
  (void) pvParameters;

  pinMode(6, OUTPUT);
  float p_1;

  for (;;)
  {
    noTone(7);
    tone(6, f_1);
  }
}

void Buzzer2(void *pvParameters)
{
  (void) pvParameters;

  pinMode(7, OUTPUT);
  float p_2;

  for (;;)
  {
    noTone(6);
    tone(7, f_2);
  }
}

void song(String s[], int oct[], int del[], int len) {
  for (int i = 0; i < len; i++) {
    tone(6, note(s[i], oct[i]));
    delay(del[i]);
    noTone(6);
  }
}

float note(String n, int oct) {
  String notes[] = {"A", "A#", "Bb", "B", "C", "C#", "Db", "D", "D#", "Eb", "E", "F", "F#", "Gb", "G", "G#", "Ab"};
  float freqs[] = {55.0, 58.27, 58.27, 61.74, 65.41, 69.3, 69.3, 73.42, 77.78, 77.78,82.41, 87.31, 92.5, 92.5, 98.0, 103.83, 103.83};
  float freq;
  for (int i = 0; i < 17; i++) {
    if (n == notes[i]) {
      freq = freqs[i] * rs(2, oct);
    }
  }
  return freq;
}

int rs(int a, int b) {
  int tot = 1;
  for (int i = 0; i < b; i++) {
    tot *= a;
  }
  return tot;
}
