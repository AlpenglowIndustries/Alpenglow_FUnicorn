/************************************************

  FUnicorn is a library designed for use with the Alpenglow Industries FUnicorn - an irreverent
  and educational desk gadget.  It's Arduino-compatible and should be portable to most boards running
  an ATMega328P.  It was originally written outside of the Arduino environment, so you'll see
  port calls and bitwise operations used instead of the typical digitalOutput() and pinMode() calls.
  It's just a different way of writing the same thing.  If you want to learn more about how to
  directly read and write to hardware registers, the book Make! AVR Programming by Elliot Williams
  is a great reference.  https://www.makershed.com/products/make-avr-programming

  If you're a beginner and just want examples of blinking code, scroll to
  the bottom.

  We've added comments to the code, to help you equate arduino functions to standard C with the
  built-in AVR register definitions.

  Check out the ATMEGA328P datasheet!  It tells you everything about the registers and how they're set
  and used.  https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
  Also check out iom328p.h for the Atmel-provided shortcuts/macros.  You can usually find it in:
  C:\Program Files (x86)\Arduino\hardware\tools\avr\avr\include\avr\iom328p.h

  Copyright 2018-2021, by Carrie Sundra for Alpenglow Industries
  All rights reserved.
  License: MIT

  ************************************************/

///////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////
#include "FUnicorn.h"
#include <Arduino.h>  // only used for delay()
#include <util/atomic.h>
#include <avr/sleep.h>

///////////////////////////////////////////////////
// Initialization Functions
///////////////////////////////////////////////////

FUnicorn::FUnicorn() {}

// sets all LEDs as outputs and OFF
// changes register values with bit-shifting
// (1 << MACRO)  shifts the number 00000001 by the number of positions of MACRO (defined in FUnicorn.h)
// Funicorn.h uses macros already defined in Atmel/Microchip headers included with Arduino, see above for links
// ex: (1 << FUCK) = (1 << PORTD3) = (1 << 3) = (00000001 << 3) = 00001000
//   DDRB |= (1 << FUCK); is shorthand for DDRB = DDRB | (00001000);
//   "OR"ing sets the bit in the FUCK position of the DDRB register to 1 without changing any other bits
//   "AND NOT"ing clears a bit (sets it to 0).  Ex: DDRB &= ~(1 << FUCK);
void FUnicorn::initOutputs() {
  // sets D0-D13 output values to zero
  PORTB = 0;
  PORTD = 0;
  // data direction registers D and B
  // sets all LED bank pins to outputs
  // equivalent to pinMode(pin, OUTPUT);
  // FUCK and BUTLED are on port D, everything else on port B
  // see FUnicorn.h for the pin/port mapping
  DDRD |= ((1 << FUCK) | (1 << BUTTLED));
  DDRB |= ((1 << BANG) | (1 << YOU) | (1 << HORN));
}

// sets up the external button as an interrupt on INT0
// equivalent to attachInterrupt(digitalPinToInterrupt(2), ISR(INT0_vect), FALLING);
// Interrupt functions are called ISRs (interrupt service routines)
// Built-in AVR nomenclature used, you'll find them in the example code before setup()
void FUnicorn::initButt() {
  EICRA |= (1 << ISC01);      // INT0 triggers on falling edge
  EIMSK |= (1 << INT0);       // enables INT0
};

// initializes PWM outputs on HORN and FUCK
void FUnicorn::initFuckHornTimer() {
  TCCR2A = 0;                 // clears arduino setup so we can set up PWM module ourselves
  TCCR2B = 0;
  TCCR2A |= (1 << COM2A1) | (1 << COM2B1) | (1 << WGM20);   // Phase correct PWM, 0xFF is top, 8-bit
  OCR2A = 0;                  // should start as solid off
  OCR2B = 0;
  TCCR2B |= (1 << CS20);      // enables the clock with a prescaler of 1
}

// initializes PWM outputs on BANG and YOU
void FUnicorn::initBangYouTimer() {
  TCCR1A = 0;                 // clears arduino setup so we can set up PWM module ourselves
  TCCR1B = 0;
  TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);   // Phase correct PWM, 0x00FF is top, 8-bit
  ATOMIC_BLOCK(ATOMIC_FORCEON){       // atomic operation for 16-bit register writes
    OCR1A = 0;                        // should start as solid off
    OCR1B = 0;
  }
  TCCR1B |= (1 << CS10);              // enables the clock with a prescaler of 1
}

// Calls initialization functions common to all FUnicorn demos
void FUnicorn::init() {

  initOutputs();

  //sets up PWMs to dim/pulse LEDs
  initFuckHornTimer();
  initBangYouTimer();

}

///////////////////////////////////////////////////
// Sleep Handling
// - shuts down timers
// - default sleep mode is idle, necessary for an edge-based interrupt
// - sets the sleep enable bit and puts the CPU to sleep
// - wakes up (from a button press)
// - enables the timers, initializes outputs again
///////////////////////////////////////////////////

void FUnicorn::sleep() {

// saves the status of different modules: analog comparator, adc, timers and watchdog
// then sets all main registers to 0 which disables them
  uint8_t acsr_status = ACSR;
  ACSR = 0;    // turns off analog comparator
  uint8_t adcsra_status = ADCSRA;
  ADCSRA = 0;  // turns off ADC
  uint8_t timer0_status = TCCR0B;
  uint8_t timer1_status = TCCR1B;
  uint8_t timer2_status = TCCR2B;
  TCCR0B = 0;  // turns off timers
  TCCR1B = 0;
  TCCR2B = 0;
  uint8_t watchdog_status = WDTCSR;
  WDTCSR = 0;  // turns off watchdog

// Power Reduction Register
// powers down individual modules
// might not be needed since sleep mode is power down mode

  //  PRR |= ((1 << PRTIM0) | (1 << PRTIM1) | (1 << PRTIM2) | (1 << PRADC) | (1 << PRTWI) | (1 << PRSPI) | (1 << PRUSART0));
  PRR |= ((1 << PRTIM0) | (1 << PRTIM1) | (1 << PRTIM2) | (1 << PRADC) | (1 << PRTWI) | (1 << PRSPI));
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  SMCR |= (1 << SE);    // enables sleep mode
  sleep_bod_disable();  // disables the brown-out detector reset
  sleep_cpu();
  // goes to sleep
  // ....sleeps until the button is pressed

  // wakes from sleep
  SMCR &= ~(1 << SE);   // disables sleep mode

// clears the power reduction register, disables power-down for modules
  //  PRR &= ~((1 << PRTIM0) | (1 << PRTIM1) | (1 << PRTIM2) | (1 << PRADC) | (1 << PRTWI) | (1 << PRSPI) | (1 << PRUSART0));
  PRR &= ~((1 << PRTIM0) | (1 << PRTIM1) | (1 << PRTIM2) | (1 << PRADC) | (1 << PRTWI) | (1 << PRSPI));

// sets all modules to their previously saved statuses, enables things that were enabled
  TCCR2B = timer2_status;
  TCCR1B = timer1_status;
  TCCR0B = timer0_status;
  WDTCSR = watchdog_status;
  ADCSRA = adcsra_status;
  ACSR = acsr_status;

// re-initializes the LED bank outputs
  initOutputs();

// now the code is ready to continue, everything is fully awake and operating
}

///////////////////////////////////////////////////
// Examples of Blink Routines
///////////////////////////////////////////////////

// pulses the Horn LED once
void FUnicorn::hornBlink() {

  uint8_t j;
  for (j = 0; j < 255; j++) {
    HORN_PWM = j;
    delay(2);
  }
  HORN_PWM = ON;
  delay(150);
  for (j = 254; j < 255; j--) {
    HORN_PWM = j;
    delay(2);
  }
  HORN_PWM = OFF;
  delay(150);
}

// 4-part blink routine
void FUnicorn::blinkDemo() {

  BUTTLED_ON;
  delay(500);

  // pulses FUCK to solid ON
  uint8_t i;
  for (i = 0; i < 255; i++) {
    FUCK_PWM = i;
    delay(3);
  }
  FUCK_PWM = ON;
  delay(500);

  // pulses YOU to solid ON
  for (i = 0; i < 255; i++) {
    YOU_PWM = i;
    delay(3);
  }
  YOU_PWM = ON;
  delay(500);

  // flashes BANG on/off 10 times
  for (i = 0; i < 10; i++){
    BANG_PWM = ON;
    delay(100);
    BANG_PWM = OFF;
    delay(100);
  }
  BANG_PWM = ON;

  // pulses HORN on/off 2 times, ending with solid ON
  uint8_t j;
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 255; j++) {
      HORN_PWM = j;
      delay(2);
    }
    HORN_PWM = ON;
    delay(150);
    for (j = 254; j < 255; j--) {
      HORN_PWM = j;
      delay(2);
    }
    HORN_PWM = OFF;
    delay(150);
  }
  for (j = 0; j < 255; j++) {
    HORN_PWM = j;
    delay(2);
  }
  HORN_PWM = ON;

  // everything is solid ON for 2 seconds
  delay(2000);

  // turns all LEDs off
  FUCK_OFF;
  YOU_OFF;
  BANG_OFF;
  HORN_OFF;
  BUTTLED_OFF;
}

// slowly blinks "Fuck You, You Fuck !!!"
void FUnicorn::blinkYouFuck() {

  BUTTLED_ON;
  HORN_ON;

  FUCK_ON;
  delay(500);
  FUCK_OFF;
  delay(500);
  YOU_ON;
  delay(500);
  YOU_OFF;
  delay(500);

  YOU_ON;
  delay(500);
  YOU_OFF;
  delay(500);
  FUCK_ON;
  delay(500);
  FUCK_OFF;
  delay(500);

  uint8_t i;
  for (i = 0; i < 5; i++){
    BANG_ON;
    delay(100);
    BANG_OFF;
    delay(100);
  }

  HORN_OFF;
  BUTTLED_OFF;
}

// everything flashes super fast like crazy
void FUnicorn::blinkCrazy() {

  BUTTLED_ON;

  uint8_t i;
  for (i = 0; i < 50; i++) {
    FUCK_ON;
    delay(20);
    FUCK_OFF;
    YOU_ON;
    delay(20);
    YOU_OFF;
    BANG_ON;
    delay(20);
    BANG_OFF;
    HORN_ON;
    delay(20);
    HORN_OFF;
  }

  BUTTLED_OFF;
}

// pulses the entire message
void FUnicorn::blinkAllOn() {

  BUTTLED_ON;

  // pulses all LEDs to solid ON
  uint8_t i;
  for (i = 0; i < 255; i++) {
    FUCK_PWM = i;
    YOU_PWM = i;
    BANG_PWM = i;
    HORN_PWM = i;
    delay(3);
  }

  // ensuring all LEDs are solid ON
  FUCK_ON;
  YOU_ON;
  BANG_ON;
  HORN_ON;
  delay(3000);

  // pulses all LEDs to solid OFF
  for (i = 254; i < 255; i--) {
    FUCK_PWM = i;
    YOU_PWM = i;
    BANG_PWM = i;
    HORN_PWM = i;
    delay(3);
  }
  FUCK_OFF;
  YOU_OFF;
  BANG_OFF;
  HORN_OFF;

  BUTTLED_OFF;
}

// slowly blinks "Fuck You" twice
void FUnicorn::blinkFuckYou2X() {
  BUTTLED_ON;

  uint8_t j;
  for (j = 0; j < 2; j++) {

    FUCK_ON;
    delay(350);
    YOU_ON;
    BANG_ON;
    delay(350);
    HORN_ON;
    delay(350);

    FUCK_OFF;
    YOU_OFF;
    BANG_OFF;
    HORN_OFF;

    delay(500);
  }

  BUTTLED_OFF;
}

// blinks "Fuck, You, Fuck, Fuck, You"  A good one for emphasis.
void FUnicorn::FuckYouFuckFuckYou() {

  BUTTLED_ON;
  HORN_ON;

  FUCK_ON;
  delay(750);
  FUCK_OFF;
  delay(200);

uint8_t i;
for (i = 0; i < 255; i++) {
  YOU_PWM = i;
  BANG_PWM = i;
  delay(3);
}
YOU_ON;
BANG_ON;
delay(750);

for (i = 254; i < 255; i--) {
  YOU_PWM = i;
  BANG_PWM = i;
  delay(3);
}
YOU_OFF;
BANG_OFF;

  FUCK_ON;
  delay(500);
  FUCK_OFF;
  delay(200);
  FUCK_ON;
  delay(500);
  FUCK_OFF;
  delay(200);

for (i = 0; i < 255; i++) {
  YOU_PWM = i;
  BANG_PWM = i;
  delay(3);
}
YOU_ON;
BANG_ON;
delay(750);

for (i = 254; i < 255; i--) {
  YOU_PWM = i;
  BANG_PWM = i;
  delay(3);
}
YOU_OFF;
BANG_OFF;
HORN_OFF;

BUTTLED_OFF;
}
