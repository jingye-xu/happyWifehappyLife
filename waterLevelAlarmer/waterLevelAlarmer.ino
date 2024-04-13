#define DEBUG false  //set to true for debug output, false for no debug output
#define DEBUG_SERIAL if(DEBUG)Serial

#include "pitches.h"
#define THRESHOLD 5

const int trigPin = 21;
const int echoPin = 20;
const int buzzerPin = 22;

float distance = 1000.0;
float duration; 

const int melody[] = {

  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4, 0, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4, 0
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
const int noteDurations[] = {

  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
};


void play(int buzzerPin){
  for (int thisNote = 0; thisNote < 16; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.

    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.

    int noteDuration = 1000 / noteDurations[thisNote];

    tone(buzzerPin, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.

    // the note's duration + 30% seems to work well:

    int pauseBetweenNotes = noteDuration * 1.30;

    delay(pauseBetweenNotes);

    // stop the tone playing:

    noTone(buzzerPin);

  }
}

void getDistance(int trigPin, int echoPin){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration*.0343)/2;
}

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  DEBUG_SERIAL.begin(9600);
}

void loop() {
  
  getDistance(trigPin, echoPin);
  delay(100);
  DEBUG_SERIAL.println(distance);
  if (distance < THRESHOLD){
    play(buzzerPin);
  }
  delay(2000);
}
