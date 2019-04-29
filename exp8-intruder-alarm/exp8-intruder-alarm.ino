/**
 * @file exp8-intruder-alarm.ino
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
#include <NewPing.h>
#include <Servo.h>

/*******************************************************************************
 * Constants
 ******************************************************************************/
#define US_TRIG_PIN 12 // ultrasonic trigger pin -> Arduino 12
#define US_ECHO_PIN 13 // ultrasonic echo pin -> Arduino 13
#define PIR_TRIG_PIN 2 // PIR sensor trigger pin -> Arduino 2
#define LASER_TRIG_PIN A0 // Laser/photoresistor trigger pin -> Arduino 10
#define SERVO_PIN 9 // Automated turret -> Arduino 9

#define RED_LED_PIN 3
#define GREEN_LED_PIN 4
#define BUZZER_PIN 8
#define US_MAX_DISTANCE 500
#define TRUE 1
#define FALSE 0

/* If ultrasonic sensor detects object within 15 cm, it will trigger. You can
 * change the distance to be closer/further as needed.
 */
#define US_TRIG_DISTANCE 15

/*
 * The photoresistor value that will cause the system to trigger.
 */
#define LASER_TRIG_VAL 30

/*
 * There are three types of security systems you can use: laser, PIR, and
 * ultrasonic. To change which one is active, you just need to change the
 * SECURITY_SYSTEM_TYPE value.
 */
#define SECURITY_SYSTEM_LASER 1
#define SECURITY_SYSTEM_PIR 2
#define SECURITY_SYSTEM_US 4

#define SECURITY_SYSTEM_TYPE SECURITY_SYSTEM_US

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
NewPing sonar(US_TRIG_PIN, US_ECHO_PIN, US_MAX_DISTANCE); // Library setting
Servo myservo;

/*******************************************************************************
 * Functions
 ******************************************************************************/
 void setup(void) {
   /* setup printing to computer serial console */
	Serial.begin (115200);

  /* init automated response system */
  myservo.attach(SERVO_PIN);
	pinMode(GREEN_LED_PIN, OUTPUT);
	pinMode(RED_LED_PIN, OUTPUT);
	pinMode(BUZZER_PIN, OUTPUT);

  /* setup sensor PINs */
	pinMode(US_TRIG_PIN, OUTPUT);
	pinMode(US_ECHO_PIN, INPUT);
	pinMode(PIR_TRIG_PIN, INPUT);
  pinMode(LASER_TRIG_PIN, INPUT);
} /* setup() */

void loop(void) {
  if ((SECURITY_SYSTEM_LASER == SECURITY_SYSTEM_TYPE && security_laser_triggered()) ||
      (SECURITY_SYSTEM_PIR == SECURITY_SYSTEM_TYPE && security_pir_triggered()) ||
      (SECURITY_SYSTEM_US == SECURITY_SYSTEM_TYPE && security_us_triggered())) {
      security_automated_response();
  } else { /* no motion detected */
    security_reset();
  }
} /* loop() */

/**
 * Return TRUE if the laser has been triggered (i.e. something came between the
 * beam and the photoresistor), and FALSE otherwise.
 */
int security_laser_triggered(void) {
  int laser_triggered = analogRead(LASER_TRIG_PIN);

  if (laser_triggered <= LASER_TRIG_VAL) {
      Serial.println("Laser motion detected!");
      return TRUE;
  }
  return FALSE;
} /* security_laser_triggered() */

/**
 * Return TRUE if the PIR sensor has been triggered, and FALSE otherwise.
 */
int security_pir_triggered(void) {
  /* get PIR sensor trigger state */
	int pir_triggered = digitalRead(PIR_TRIG_PIN);

  if (pir_triggered) {
    Serial.println("PIR motion detected!");
    return TRUE;
  }
  return FALSE;
} /* security_pir_triggered() */

/**
 * Return TRUE if the ultrasonic sensor has been triggered, and FALSE
 * otherwise.
  */
int security_us_triggered(void) {
  /* Compute ultrasonic sensor trigger state */
	digitalWrite(US_TRIG_PIN, LOW);
	delayMicroseconds(2);
	digitalWrite(US_TRIG_PIN, HIGH); // Trig pin sends a ping
	delayMicroseconds(10);
	digitalWrite(US_TRIG_PIN, LOW);
	int us_duration = pulseIn(US_ECHO_PIN, HIGH); // Echo receives the ping
	int us_distance = (us_duration / 2) / 29.1;
	Serial.print(us_distance);
	Serial.println(" cm");

  if (us_distance <= US_TRIG_DISTANCE) {
    Serial.println("US motion detected!");
    return TRUE;
  }
  return FALSE;
} /* security_us_triggered() */

/**
 * Execute the security system's automated response of flashing the red LED,
 * buzzing, and moving the turret back and forth.
 */
void security_automated_response(void) {
  digitalWrite(GREEN_LED_PIN, LOW); // Turn off green LED
  digitalWrite(RED_LED_PIN, HIGH); // Turn on red LED
  myservo.write(180); // Move servo arm 180 degrees
  play_tone(300, 160);  // Turn the piezo ON
  digitalWrite(RED_LED_PIN, LOW); // Light the red LED
  digitalWrite(BUZZER_PIN, LOW);
  myservo.write(90);
  digitalWrite(RED_LED_PIN, HIGH);
  myservo.write(0);
  digitalWrite(RED_LED_PIN, LOW);
  myservo.write(90);
  delay(500);
} /* security_automated_response() */

/**
 * After the system is no longer being triggered, turn the green LED back on and
 * stop moving the servo.
  */
void security_reset(void) {
  digitalWrite(RED_LED_PIN, LOW); // Turn off red LED
  digitalWrite(GREEN_LED_PIN, HIGH); // Turn on green LED
  myservo.write(90);
  play_tone(0, 0); // Turn off the piezo
} /* security_reset() */

void play_tone(long duration, int freq) { // Duration in ms,
	// frequency in Hz
	duration *= 1000;
	int period = (1.0 / freq) * 1000000;
	long elapsed_time = 0;
	while (elapsed_time < duration) {
		digitalWrite(BUZZER_PIN, HIGH);
		delayMicroseconds(period / 2);
		digitalWrite(BUZZER_PIN, LOW);
		delayMicroseconds(period / 2);
		elapsed_time += (period);
  } /* while() */

} /* play_tone() */
