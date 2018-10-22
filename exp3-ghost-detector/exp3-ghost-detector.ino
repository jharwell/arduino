/**
 * @file ghost-detector.ino
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
#define NUM_READINGS 25 /* increase for more data smoothing */
#define SENSE_LIMIT 1023 /* raise to decrease antenna sensitivity (up to
                            1023 max)  */

int antenna_pin = 5;
int antenna_reading;

int led1 = 2;
int led2 = 3;
int led3 = 4;
int led4 = 5;
int led5 = 6;
int led6 = 7;
int led7 = 8;
int led8 = 9;
int led9 = 10;

int buzzer = 13;

int readings[NUM_READINGS];
int reading_index = 0;
int reading_sum = 0;
int reading_average = 0;

/*******************************************************************************
 * Functions
 ******************************************************************************/
/* 1-time setup code that runs during initialization. */
void setup(void) {

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);
  pinMode(led7, OUTPUT);
  pinMode(led8, OUTPUT);
  pinMode(led9, OUTPUT);

  Serial.begin(9600);

  memset(readings, sizeof(readings), 0);
} /* setup() */

/* Main code, which will run forever. */
void loop(void) {

  double val = analogRead(antenna_pin);
  if (val >= 1) {

    /*
     * If the reading is higher than the current sense limit, clamp it to the
     * limit.
     */
    val = constrain(val, 1, SENSE_LIMIT);

    /* Remap the constrained value to the range [1, 1023] */
    val = map(val, 1, SENSE_LIMIT, 1, 1023);

    reading_sum -= readings[reading_index];
    readings[reading_index] = val;
    reading_sum += readings[reading_index];
    reading_index = reading_index + 1;

    if (reading_index >= NUM_READINGS) {
      reading_index = 0;
    }

    reading_average = reading_sum / NUM_READINGS;

    digitalWrite(led1, reading_average > 100 ? HIGH : LOW);
    digitalWrite(led2, reading_average > 200 ? HIGH : LOW);
    digitalWrite(led3, reading_average > 300 ? HIGH : LOW);
    digitalWrite(led4, reading_average > 400 ? HIGH : LOW);
    digitalWrite(led5, reading_average > 500 ? HIGH : LOW);
    digitalWrite(led6, reading_average > 600 ? HIGH : LOW);
    digitalWrite(led7, reading_average > 700 ? HIGH : LOW);
    digitalWrite(led8, reading_average > 800 ? HIGH : LOW);
    digitalWrite(led9, reading_average > 900 ? HIGH : LOW);

    if (reading_average > 900) {
      tone(buzzer, 1000);
    } else {
      noTone(buzzer);
    }
    Serial.println(reading_average); // use output to aid in calibrating 
  }
} /* loop() */
