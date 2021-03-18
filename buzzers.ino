/*
  Multiple tone player

  Plays multiple tones on multiple pins in sequence

  circuit:
  - three 8 ohm speakers on digital pins 6, 7, and 8

  created 8 Mar 2010
  by Tom Igoe
  based on a snippet from Greg Borenstein

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Tone4
*/

void setup() {

}

void loop() {
  //tone(6, note("Bb", 3), 100);
  String notes[] = {"Bb", "C", "D", "Eb", "F"};
  int oct[] = {3,3,3,3,3};
  int del[] = {200, 200, 200, 200, 400};
  song(notes, oct, del, 5);
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
