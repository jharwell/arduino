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
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <string.h>

/*******************************************************************************
 * Structure Definitions
 ******************************************************************************/
/**
 * @brief A representation of a 3 digit # that contains both numeric and string
 * representations.
 */

struct numstr {
  /**
   * @brief The array of chars that constitute the string representation of the
   * #.
   */
  char chars[4];

  /**
   * @brief How many chars in the chars array are currently valid.
   */
  int   len;

  /**
   * @brief The numeric value of the #.
   */
  int   val;
};

struct clues {
  const char** strs;
  int len;
};

/*******************************************************************************
 * Constant Definitions
 ******************************************************************************/
#define MAX_GUESSES 10
#define LCD_PIN 6
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4
#define XSTR(a) _XSTR(a)
#define _XSTR(a) #a

const char* kCorrect = "You Got It!";
const char* kFermi = "Fermi";
const char* kPico = "Pico";
const char* kBAGELS = "BAGELS";
const char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte row_pins[KEYPAD_ROWS] = {2,3,4,5};
byte col_pins[KEYPAD_COLS] {6,7,8,9};

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // Pins attached to LCD screen
Keypad keypad = Keypad(makeKeymap(keys),
                row_pins,
                col_pins,
                KEYPAD_ROWS,
                KEYPAD_COLS);

/*******************************************************************************
 * Functions
 ******************************************************************************/
/**
 * @brief Returns a random number with the specified # of digits
 */
int gen_secret_num(int n_digits) {
  return random() % ((int)pow(10, n_digits));
} /* gen_secret_num() */

/**
 * @brief Returns the integer of the nth digits in the specified #. Digits are 0
 * based, and the numbering starts at the far right of a # (e.g. digit 0 of 456
 * is 6).
 */
int nth_digit(int num, int digit) {
  while (digit--) {
    num /= 10;
  } /* while() */
  return (num % 10) + '0';
} /* nth_digit() */

/**
 * @brief Returns TRUE if any of the digits in num is digit, and FALSE
 * otherwise.
 */
int contains_digit(int num, int digit) {
  return nth_digit(num, 0) == digit ||
      nth_digit(num, 1) == digit ||
      nth_digit(num, 2) == digit;
} /* contains_digit() */
/**
 * @brief Returns a string with the pico, fermi, bagels clues to the user.
 */
struct clues get_clues(const struct numstr* const guess, int secret_num) {
  struct clues result;
  result.len = 0;
  if (guess->val == secret_num) {
    result.strs[result.len++] = kCorrect;
    return result;
  }

  for (int i = 0; i < 3; ++i) {
    if (nth_digit(guess->val, i) == nth_digit(secret_num, i)) {
      result.strs[result.len++] = kFermi;
    } else if (contains_digit(guess->val, nth_digit(secret_num, i))) {
      result.strs[result.len++] = kPico;
    }
  } /* for(i..) */

  if (0 == result.len) {
    result.strs[result.len++] = kBAGELS;
  }
  return result;
} /* get_clues() */

void setup(void) {
  lcd.begin(16, 2);
} /* setup() */

struct numstr get_guess(void) {
  struct numstr guess;
  guess.len = 0;
  while(1) {
    char key = keypad.getKey();
    if (NO_KEY == key) {
      continue;
    }
    guess.chars[guess.len++] = key;
    if (3 == guess.len) {
      char val[3];
      for (int i = 0; i < 3; ++i) {
        val[i] = guess.chars[i];
      } /* for(i..) */
      guess.val = atoi(val);
      break;
    }
  } /* while(1) */
  return guess;
} /* get_guess() */

void loop(void) {
  int secret_num = gen_secret_num(3);
  lcd.setCursor(0, 0);
  lcd.print("I have thought up a 3-digit number. You have " XSTR(MAX_GUESSES) "guesses to get it.");

  int n_guesses = 1;
  struct numstr guess;
  while (n_guesses <= MAX_GUESSES) {
    struct numstr guess = get_guess();

    /* They guessed the number! */
    if (guess.val == secret_num) {
      lcd.print(guess.chars[0]);
      break;
    }

    /* They guessed wrong--calculate and print their clues */
    struct clues clues = get_clues(&guess, secret_num);
    lcd.setCursor(0, 0);
    for (int i = 0; i < clues.len; ++i) {
      lcd.print(clues.strs[i]);
      lcd.print(",");
    } /* for(i..) */

    ++n_guesses;
    lcd.setCursor(0, 1);
    if (n_guesses > MAX_GUESSES) {
      lcd.print("You ran out of guesses. The answer was:");
      lcd.print(secret_num);
      break;
    } else {
      lcd.print(MAX_GUESSES - n_guesses);
      lcd.print(" guesses left.");
    }
  } /* while(guesses...) */
} /* loop() */
