
/************************************************

  FUnicorn is a library designed for use with the Alpenglow Industries FUnicorn - an irreverent
  and educational desk gadget.  It's Arduino-compatible and should be portable to most boards running
  an ATMega328P.

  Copyright 2018, by Carrie Sundra for Alpenglow Industries
  All rights reserved.
  License: MIT

  ************************************************/

#ifndef FUNICORN_H
#define FUNICORN_H

// shorthand to access LED pins
// these are hardwired on the PCB, so you can't change them
#define HORN  PORTB3  // arduino D11
#define BANG  PORTB1  // arduino D9
#define FUCK  PORTD3  // arduino D3
#define YOU   PORTB2  // arduino D10
#define BUTT  PIND2   // for reading value of button pin
#define BUTTPIN PORTD2  // arduino D2
#define BUTTLED PORTD4  // arduino D4

// shorthand to access PWM timer registers for each bank of LEDs
// these are used to make the LEDs dim and appear to pulse
// range is 0-255
// Usage ex: FUCK_PWM = 127;    // sets FUCK to 50% brightness
// equivalent to: OCR2B = 127;
// equivalent to: analogWrite(3, 127);  // FUCK is arduino pin D3
// see FUnicorn.cpp for examples of slow pulsing using for loops
#define FUCK_PWM  OCR2B
#define YOU_PWM   OCR1B
#define BANG_PWM  OCR1A
#define HORN_PWM  OCR2A

// shorthand to define ON and OFF
// ON is 255, full brightness, PWM duty cycle is 100%, solid high
// OFF is 0, zero brightness, PWM duty cycle is 0%, solid low
#define ON      0xFF  // 255 in dec
#define OFF     0x00  // 0 in dec

// shorthand to turn banks of LEDs full on and off via PWM
// usage ex: FUCK_ON;  // turns FUCK word fully on
// useful if you just want to blink them
#define FUCK_ON   (FUCK_PWM = ON)
#define FUCK_OFF  (FUCK_PWM = OFF)
#define YOU_ON    (YOU_PWM = ON)
#define YOU_OFF   (YOU_PWM = OFF)
#define BANG_ON   (BANG_PWM = ON)
#define BANG_OFF  (BANG_PWM = OFF)
#define HORN_ON   (HORN_PWM = ON)
#define HORN_OFF  (HORN_PWM = OFF)

// shorthand for button handling
// the big red button has an internal LED which can be turned on and off
// Usage ex: BUTTLED_OFF;  // turns off the LED in the big red button
#define BUTTLED_ON  (PORTD |= (1 << BUTTLED))
#define BUTTLED_OFF (PORTD &= ~(1 << BUTTLED))
// useful for checking button state, ex: if (BUTT_IS_PRESSED) {do something}
#define BUTT_IS_PRESSED bit_is_clear(PIND, BUTT)

// time period after initial button press that bounces are ignored
// necessary if the button is used to interrupt the processor (or wake it from sleep)
#define DEBOUNCE  30

// defines modes of operation for ship code
#define CAP_AND_BUTT 0    // capacitive touch and external button enabled
#define LOW_PWR_BUTT 1    // external button only with sleep mode, good for battery power

class FUnicorn {
public:
  FUnicorn();
  // initialization functions
  void initButt(void);             // sets up button pin to be an interrupt
  void init(void);                 // initializes all of the above

  // sleep function
  void sleep(void);

  // built-in blink patterns
  // check these out in FUnicorn.cpp and use them as examples to build your own!
  // Usage ex: fun.hornBlink();
  void hornBlink(void);
  void blinkDemo(void);
  void blinkCrazy(void);
  void blinkYouFuck(void);
  void blinkFuckYou2X(void);
  void blinkAllOn(void);
  void FuckYouFuckFuckYou(void);

private:
  // used by init();
  void initOutputs(void);          // sets up all LED pins
  void initFuckHornTimer(void);    // sets up timer module used for Fuck and Horn LEDs
  void initBangYouTimer(void);     // sets up timer module used for Bang and You LEDs
};

#endif
