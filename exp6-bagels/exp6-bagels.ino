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
 * Constant Definitions
 ******************************************************************************/
/*
 * The # of digits in the secret # the arduino will generate.
 */
#define NUM_DIGITS 3

/*
 * The maximum # of guesses to guess the secret #.
 */
#define MAX_GUESSES 10

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 3

const char* kCorrect = "You Got It!";
const char* kFermi = "Fermi";
const char* kPico = "Pico";
const char* kBAGELS = "BAGELS";
const char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte row_pins[KEYPAD_ROWS] = {0,1,2,3};
byte col_pins[KEYPAD_COLS] {4,5,6};

/*******************************************************************************
 * Macros
 ******************************************************************************/
#define XSTR(a) _XSTR(a)
#define _XSTR(a) #a

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
  char chars[NUM_DIGITS+1];

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
  const char* strs[NUM_DIGITS];
  int len;
};

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
LiquidCrystal lcd(8,9,10,11,12,13); // Pins attached to LCD screen
Keypad keypad = Keypad(makeKeymap(keys),
                row_pins,
                col_pins,
                KEYPAD_ROWS,
                KEYPAD_COLS);

/*******************************************************************************
 * Functions
 ******************************************************************************/
void setup(void) {
  lcd.begin(16, 2);
  randomSeed(analogRead(0));
} /* setup() */

void loop(void) {
  int secret_num = gen_secret_num(NUM_DIGITS);
  lcd.setCursor(0, 0);
  lcd.print("I'm thinking of");
  lcd.setCursor(0, 1);
  lcd.print("a " XSTR(NUM_DIGITS) "-digit number");
  delay(3000);
  lcd.clear();
  lcd.print("Try to guess it!");
  delay(3000);
  lcd.clear();

  play_game(secret_num);
} /* loop() */

void play_game(int secret_num) {
  int n_guesses = 1;
  struct numstr guess;
  while (n_guesses <= MAX_GUESSES) {
    int result = play_round(secret_num, n_guesses);
    if (result) {
      break;
    }
    n_guesses++;
  } /* while(guesses...) */
} /* play_game() */

int play_round(int secret_num, int n_guesses) {
  lcd.setCursor(0,0);
  lcd.print(MAX_GUESSES - (n_guesses - 1));
  lcd.print(" guesses left!");
  struct numstr guess = get_guess();

  /* Calculate and print clues */
  struct clues clues = get_clues(&guess, secret_num);
  lcd.setCursor(0, 0);
  lcd.print("You guessed ");
  lcd.print(guess.val);
  lcd.print(": ");
  lcd.setCursor(0, 1);
  for (int i = 0; i < clues.len; ++i) {
    lcd.print(clues.strs[i]);
    if (i < clues.len - 1) {
      lcd.print(",");
    }
  } /* for(i..) */
  delay(5000);
  lcd.clear();

  /* They guessed the number! */
  if (guess.val == secret_num) {
    lcd.print(guess.chars[0]);
    return 1;
  }

  if (n_guesses >= MAX_GUESSES) {
    lcd.setCursor(0, 0);
    lcd.print("You lose!");
    lcd.setCursor(0, 1);
    lcd.print("My # was: ");
    lcd.print(secret_num);
    delay(5000);
    return 1;
  }
  return 0;
} /* play_round() */

/**
 * @brief Returns a set of strings with the pico, fermi, bagels clues to the
 * user.
 */
struct clues get_clues(const struct numstr* const guess, int secret_num) {
  struct clues result;
  result.len = 0;
  lcd.setCursor(0,1);

  if (guess->val == secret_num) {
    result.strs[result.len++] = kCorrect;
    return result;
  }

  for (int i = NUM_DIGITS - 1; i >= 0; --i) {
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


struct numstr get_guess(void) {
  struct numstr guess;
  guess.len = 0;
  while(1) {
    char key = keypad.getKey();
    if (NO_KEY == key) {
      continue;
    }
    guess.chars[guess.len++] = key;
    if (NUM_DIGITS == guess.len) {
      char val[NUM_DIGITS];
      for (int i = 0; i < NUM_DIGITS; ++i) {
        val[i] = guess.chars[i];
      } /* for(i..) */
      guess.val = atoi(val);
      break;
    }
  } /* while(1) */
  return guess;
} /* get_guess() */

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
 * @brief Returns TRUE if any of the digits in num is 'digit', and FALSE
 * otherwise.
 */
int contains_digit(int num, int digit) {
  int contains = 0;
  for (size_t i = 0; i < NUM_DIGITS; ++i) {
    contains |= nth_digit(num, i) == digit;
  } /* for(i..) */
  return contains;
} /* contains_digit() */

