/**
 * @file exp7-joystick-laser.ino
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
 * Includes
 ******************************************************************************/
#include <Servo.h>

/*******************************************************************************
 * Constant Definitions
 ******************************************************************************/
#define XSERVO_PIN 10 /* Servo pin that will control X motion (yaw) */
#define YSERVO_PIN 9 /* Servo pin that will control Y motion (pitch) */

#define LASER_PIN 7        /* Pin connected to laser output */
#define LASER_BUTTON_PIN 6 /* Pin for button to toggle laser on/off */

#define XPOS_PIN A0 /* Input pin for the joystick X potentiometer */
#define YPOS_PIN A1 /*  Input pin for the joystick Y potentiometer */

#define DEBOUNCE_ENABLE 1 /* Should we debounce button presses? */

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
/* servo variables */
Servo xservo, yservo;
int xpos = 0;
int ypos = 0;

/* laser variables */
unsigned long last_debounce_time = 0;
int laser_button_state = HIGH;
int laser_on = LOW;

/*******************************************************************************
 * Functions
 ******************************************************************************/
/**
 * Setup function: runs once when the sketch is loaded/Arduino is powered on
 */
void setup(void) {
  pinMode(LASER_PIN,OUTPUT);
  pinMode(LASER_BUTTON_PIN,INPUT);
  digitalWrite(LASER_BUTTON_PIN,HIGH);  /* Enable the Arduino pullup for this pin */
  Serial.begin(9600);
  xservo.attach(XSERVO_PIN);
  yservo.attach(YSERVO_PIN);
} /* setup() */


/**
 * Loop function: runs over and over infinitely after the setup() function
 * finishes until the loaded sketch is changed or the Arduino is powered off.
 */
void loop(void) {

  /* Do everything related to the laser we need to do */
  handle_laser(DEBOUNCE_ENABLE);

  /* Do everything related to reading joystick and moving the servos we need to do */
  handle_servos();

  /* Wait a 15ms before running the loop again */
  delay(100);
} /* loop() */

void handle_servos() {
  /*
   * Read the analog x position location of the joystick and map
   * the value (which is 0-1023) to the 0-180 range for the servo.
   */
  xpos = analogRead(XPOS_PIN);
  xpos = map(xpos, 0, 1023, 0, 180);
  /*
   * Read the analog Y position location of the joystick and map
   * the value (which is 0-1023) to the 0-180 range for the servo.
   */
  ypos = analogRead(YPOS_PIN);
  ypos = map(ypos, 0, 1023, 0, 180);
  /* move the yaw servo to the X position the joystick is at */
  xservo.write(xpos);

  /* move the pitch servo to the Y position the joystick is at */
  yservo.write(ypos);
} /* handle_servos() */

/**
 * Perform debouncing of the button that controls the laser and toggle the laser
 * appropriately.
 *
 * The button press is active low , so if it is pressed we will flip the laser
 * state from on -> off or off -> on. To read the button signal we can:
 *
 * 1. Read the raw value from the button, and use as is (possibly noisy).
 * 2. We can "debounce" the button to prevent it momentary noises/presses from
 * erroneously tripping the laser.
 */
int handle_laser(int debounce_enable) {
  unsigned long debounce_delay = 100;

  /* sample the state of the button--is it pressed or not? */
  int reading = digitalRead(LASER_BUTTON_PIN);

  /* filter out any noise by setting a timer */
  if ((debounce_enable && (millis() - last_debounce_time) > debounce_delay) ||
      !debounce_enable) {

    /* If the button is actually pressed, turn on the laser if it was off  */
    if (reading == LOW && laser_on == LOW) {
      digitalWrite(LASER_PIN, HIGH);
      laser_on = HIGH;
      last_debounce_time = millis();
    }
    /* If the button is actually pressed, turn off the laser if it was on  */
    else if (reading == LOW && laser_on == HIGH) {
      digitalWrite(LASER_PIN, LOW);
      laser_on = LOW;
      last_debounce_time = millis();
    }
  }

} /* handle_laser() */
