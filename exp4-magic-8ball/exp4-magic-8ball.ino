/**
 * @file exp4-magic-8ball.ino
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
#include <LiquidCrystal.h>

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // Pins attached to LCD screen

/*******************************************************************************
 * Constant Definitions
 ******************************************************************************/
const int sw_pin = 6; // Pin attached to push button switch

/*******************************************************************************
 * Functions
 ******************************************************************************/
/* 1-time setup code that runs during initialization. */
void setup() {
  lcd.begin(16, 2);
  pinMode(sw_pin, INPUT);

  lcd.print("FORTUNE TELLER"); // Print this on line 1
  lcd.setCursor(0, 1);
  lcd.print("Ask a Question"); // Print this on line 2
  Serial.begin(9600);
} /* setup() */

/* Main code, which will run forever. */
void loop() {
  int sw_state = digitalRead(sw_pin); // Read push button switch pin
  int reply;

  /* If the pushbutton is pushed, print the fortune */
  if (sw_state == LOW) {
      reply = random(8); // Reply is 1 of 8 random cases as below
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("The answer is: "); // Print this to the screen
      lcd.setCursor(0, 1);
      switch (reply) { // Reply will be one of the following cases
      case 0:
        lcd.print("Yes");
        break;
      case 1:
        lcd.print("Probably");
        break;
      case 2:
        lcd.print("Definitely");
        break;
      case 3:
        lcd.print("Don't be silly");
        break;
      case 4:
        lcd.print("Of course");
        break;
      case 5:
        lcd.print("Ask again");
        break;
      case 6:
        lcd.print("Doubtful");
        break;
      case 7:
        lcd.print("No");
        break;
    }
  }
} /* loop() */
