
#include "pitches.h" //add Equivalent frequency for musical note
#include "themes.h" //add Note vale and duration 


void Play_Pirates()
{ 
  for (int thisNote = 0; thisNote < (sizeof(Pirates_note)/sizeof(int)); thisNote++) {

    int noteDuration = 1000 / Pirates_duration[thisNote];//convert duration to time delay
    tone(5, Pirates_note[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.05; //Here 1.05 is tempo, increase to play it slower
    delay(pauseBetweenNotes);
    noTone(5); //stop music on pin 8 
    }
}

void Play_CrazyFrog()
{
  for (int thisNote = 0; thisNote < (sizeof(CrazyFrog_note)/sizeof(int)); thisNote++) {

    int noteDuration = 1000 / CrazyFrog_duration[thisNote]; //convert duration to time delay
    tone(5, CrazyFrog_note[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;//Here 1.30 is tempo, decrease to play it faster
    delay(pauseBetweenNotes);
    noTone(5); //stop music on pin 8 
    }
}

void Play_MarioUW()
{
    for (int thisNote = 0; thisNote < (sizeof(MarioUW_note)/sizeof(int)); thisNote++) {

    int noteDuration = 1000 / MarioUW_duration[thisNote];//convert duration to time delay
    tone(5, MarioUW_note[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.80;
    delay(pauseBetweenNotes);
    noTone(5); //stop music on pin 8 
    }
}

void Play_Titanic()
{
    for (int thisNote = 0; thisNote < (sizeof(Titanic_note)/sizeof(int)); thisNote++) {

    int noteDuration = 1000 / Titanic_duration[thisNote];//convert duration to time delay
    tone(5, Titanic_note[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 2.70;
    delay(pauseBetweenNotes);
    noTone(5); //stop music on pin 8 
    }
}

void setup() {
pinMode(2, INPUT_PULLUP); //Button 1 with internal pull up
pinMode(3, INPUT_PULLUP); //Button 2 with internal pull up
pinMode(4, INPUT_PULLUP); //Button 3 with internal pull up
pinMode(5, INPUT_PULLUP); //Button 4 with internal pull up
Serial.begin(9600);
}

void loop() {

    Play_CrazyFrog();

    
}
