/**
 * @file exp5-strobe-melody.ino
 *
 * This is free and unencumbered software released into the public domain.
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org/>
 */

/*******************************************************************************
 * Constant Definitions
 ******************************************************************************/
#define POT_PIN A0 // analog input pin connected to potentiometer
#define SPEAKER_PIN 13   // digital output for piezo buzzer

const int note_leds[8] = {5,6,7,8,9,10,11,12};

/*
 * Definitions for twinkle twinkle little star
 */
#if 0
#define MELODY_LENGTH 15 // # of notes in melody

const char note_names[8] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
int note_pitches[8] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
char notes[MELODY_LENGTH] = "ccggaagffeeddc";
int beats[MELODY_LENGTH] = {1,1,1,1,1,1,2,1,1,1,1,1,1,2,4};

/*
 * Definitions for Harry potter theme
 */
#elif 1

#define MELODY_LENGTH 15 // # of notes in melody
const char note_names[8] = { 'b', 'e', 'g', 'f', 'B', 'A', 'F', 'E' };
char notes[MELODY_LENGTH] = "begfeBAFegfEfb";
int beats[MELODY_LENGTH] = {2,3,1,2,4,2,5,5,3,1,2,4,2,5};
int note_pitches[8] = { 1014, // b
                        1519, // e
                        1275, // g
                        1432, // f
                        506, // B2
                        568, // A2
                        1351, // D#
                        1608, // Eb
};

#endif

/*******************************************************************************
 * Functions
 ******************************************************************************/
/* 1-time setup code that runs during initialization. */
void setup(void) {
  for (int i = 0; i < 8; ++i) {
    pinMode(note_leds[i], OUTPUT);
  } /* for(i..) */
  pinMode(SPEAKER_PIN, OUTPUT);
  Serial.begin(9600);
} /* setup() */

/* Main code, which will loop/run forever. */
void loop(void) {
  /*
   * First, read the raw analog value from the potentiometer. This will be
   * something in the range [0,1023], which we map to the range of tempos we
   * want our melody to be able to play at, which is [200, 400].
   */
  int pot_val = analogRead(POT_PIN);
  int tempo = map(pot_val, 0, 1023, 200, 400);
   for (int i = 0; i < MELODY_LENGTH; ++i) {
    int led;
    if (notes[i] == ' ') {
      delay(beats[i] * tempo);
    } else {
      led = select_led(notes[i]);
      digitalWrite(led, HIGH);
      play_note(notes[i], beats[i] * tempo);
    }

    /* pause between notes so our melody sounds nice */
    delay(tempo/ 2);

    /* turn off LED for this note now that it has been played */
    digitalWrite(led, LOW);
  } /* for(i..) */
} /* loop() */

int select_led(char note) {
  for (size_t i = 0; i < 8; ++i) {
    if (note_names[i] == note) {
      return note_leds[i];
    }
  } /* for(i..) */

} /* select_led() */
/**
 * @brief Given a frequency and a duration, cause corresponding tone/note to be
 * played.
 */
void play_tone(int frequency, int duration) {
  for (long i = 0; i < duration * 1000L; i += frequency * 2) {
    digitalWrite(SPEAKER_PIN, HIGH);
    delayMicroseconds(frequency);
    digitalWrite(SPEAKER_PIN, LOW);
    delayMicroseconds(frequency);
  }
} /* play_tone() */

/**
 * @brief Play the specified musical note.
 *
 * @param note The note name (a,b,c,d,e,f).
 * @param duration How long to play the specified note for, specified in
 *                 microseconds.
 */
void play_note(char note, int duration) {
  for (int i = 0; i < 8; ++i) {
    if (note_names[i] == note) {
      play_tone(note_pitches[i], duration);
    }
  } /* for(i..) */
} /* play_note() */
