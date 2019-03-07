/**
 * @file exp6-bagels.ino
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
#define XSERVO_PIN 10 /* Servo pin that will control X motion */
#define YSERVO_PIN 9 /* Servo pin that will control Y motion */

#define LASER_PIN 6 /* pin connected to laser output */
#define LASER_BUTTON_PIN 7 /* pin for button to toggle laser on/off */

#define XPOS_PIN A0 /* select the input pin for the joystick X potentiometer */
#define YPOS_PIN A1 /* select the input pin for the joystick Y potentiometer */

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
/* servo variables */
Servo xservo, yservo;
int xpos = 0;
int ypos = 0;

/* laser variables */
int laser_on = 0;
int button_press = 0;
unsigned long last_debounce_time = 0;

/*******************************************************************************
 * Functions
 ******************************************************************************/

/**
 * @brief Perform debouncing of the button that controls the laser.
 *
 * @param last_debounce The last time (in milliseconds) that we checked the
 *                      button value to see if it has debounced yet.
 * @param last_reading The previous reading of the button.
 *
 * @return 1 if the button has been debounced, 0 otherwise.
 */
int button_debounce(int last_debounce, int last_reading) {
  unsigned long debounce_delay = 50;

  int reading = digitalRead(LASER_BUTTON_PIN);

  /* If the switch changed, due to noise or pressing reset the debounce timer */
  if (reading != last_reading) {
    last_debounce = millis();
  }

  /*
   * Whatever the reading is at, it's been there for longer than the debounce
   * delay, so take it as the actual current state
   */
  if ((millis() - last_debounce) >= debounce_delay && reading == last_reading) {
    return 1;
  }
  return 0;
} /* button_debounce() */

void setup(void) {
  pinMode(LASER_PIN,OUTPUT);
  pinMode(LASER_BUTTON_PIN,INPUT);
  digitalWrite(LASER_BUTTON_PIN,HIGH);  /* Enable the Arduino pullup for this pin */

  xservo.attach(XSERVO_PIN);
  yservo.attach(YSERVO_PIN);
} /* setup() */

void loop(void) {
  /*
   * The button press is active low , so if it is pressed we will flip the laser
   * state from on -> off or off -> on. To read the button signal we can:
   *
   * 1. Read the raw value from the button, and use as is (possibly noisy).
   * 2. We can "debounce" the button to prevent it momentary noises/presses from
   * erroneously tripping the laser.
   */
  button_press = digitalRead(LASER_BUTTON_PIN);
  int pressed = button_debounce(last_debounce_time, button_press);

  if ((button_press == LOW) && pressed) {
    laser_on = !laser_on;
  }
  digitalWrite(LASER_PIN, laser_on); //Turn Laser on

  xpos = analogRead(XPOS_PIN);     // read and store the x position location of the joystick

  xpos = map(xpos,0,1023,0,180);  //map the analog read x values to the 0-180 servo values
  ypos = analogRead(YPOS_PIN);     // read and store the y position of the joystick

  ypos = map(ypos,0,1023,0,180);  //map the analog read y values to the 0-180 servo values

  xservo.write(xpos);             // move the X Location servo to the x position the joystick is at
  yservo.write(ypos);             // move the Y Location servo to the y position the joystick is at
  delay(15);
} /* loop() */
