
/************************************************

  FUnicorn is a library designed for use with the Alpenglow Industries Fuck Younicorn - an irreverent
  and educational desk gadget.  It's Arduino-compatible and should be portable to most boards running
  an ATMega328P.

  Copyright 2018, by Carrie Sundra for Alpenglow Industries
  All rights reserved.
  License: MIT

  ************************************************/

#ifndef FUNICORN_H
#define FUNICORN_H

// shorthand to access LED pins
#define HORN  PORTB3
#define BANG  PORTB1
#define FUCK  PORTD3
#define YOU   PORTB2
#define BUTT  PIND2
#define BUTTPIN PORTD2
#define BUTTLED PORTD4

// shorthand to access PWM timer registers for each LED pin
// these are used to make the LEDs dim and appear to pulse
#define FUCK_PWM  OCR2B
#define YOU_PWM   OCR1B
#define BANG_PWM  OCR1A
#define HORN_PWM  OCR2A

#define ON      0xFF
#define OFF     0x00

// shorthand to turn LED pins solid on or solid off only
// if used, uncomment this section and comment out section below
// also, comment out the 2 timer initializations in initFUnicorn()
// #define FUCK_ON    (PORTD |= (1 << FUCK))
// #define FUCK_OFF   (PORTD &= ~(1 << FUCK))
// #define YOU_ON     (PORTB |= (1 << YOU))
// #define YOU_OFF    (PORTB &= ~(1 << YOU))
// #define BANG_ON    (PORTB |= (1 << BANG))
// #define BANG_OFF   (PORTB &= ~(1 << BANG))
// #define HORN_ON    (PORTB |= (1 << HORN))
// #define HORN_OFF   (PORTB &= ~(1 << HORN))
#define FUCK_ON   (FUCK_PWM = ON)
#define FUCK_OFF  (FUCK_PWM = OFF)
#define YOU_ON    (YOU_PWM = ON)
#define YOU_OFF   (YOU_PWM = OFF)
#define BANG_ON   (BANG_PWM = ON)
#define BANG_OFF  (BANG_PWM = OFF)
#define HORN_ON   (HORN_PWM = ON)
#define HORN_OFF  (HORN_PWM = OFF)

// shorthand for button handling
#define BUTTLED_ON  (PORTD |= (1 << BUTTLED))
#define BUTTLED_OFF (PORTD &= ~(1 << BUTTLED))
#define BUTT_IS_PRESSED bit_is_clear(PIND, BUTT)

// time period after initial button press that bounces are ignored
// necessary if the button is used to interrupt the processor (or wake it from sleep)
#define DEBOUNCE  30

// initialization functions
void initOutputs (void);          // sets up all LED pins
void initButt (void);             // sets up button pin to be an interrupt
void initFuckHornTimer (void);    // sets up timer module used for Fuck and Horn LEDs
void initBangYouTimer (void);     // sets up timer module used for Bang and You LEDs
void initFUnicorn (void);         // initializes all of the above

// sleep function
void gotoSleep (void);

// blink pattern functions
void blinkDemo (void);
void blinkCrazy (void);
void blinkYouFuck (void);
void blinkFuckYou2X (void);
void blinkAllOn (void);

#endif
