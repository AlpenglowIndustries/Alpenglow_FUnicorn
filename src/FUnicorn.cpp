/************************************************

  FUnicorn is a library designed for use with the Alpenglow Industries Fuck Younicorn - an irreverent
  and educational desk gadget.  It's Arduino-compatible and should be portable to most boards running
  an ATMega328P.

  Copyright 2018, by Carrie Sundra for Alpenglow Industries
  All rights reserved.
  License: MIT

  ************************************************/

// Are you a beginner?  Scroll down to the Blink Routines section, that's the fun part.

///////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////
#include "FUnicorn.h"
#include <Arduino.h>
#include <util/atomic.h>
#include <avr/sleep.h>

///////////////////////////////////////////////////
// Initialization Functions
///////////////////////////////////////////////////
void initOutputs (void) {
  // sets the following to outputs
  PORTB = 0;
  PORTD = 0;
  DDRD |= ((1 << FUCK) | (1 << BUTTLED));
  DDRB |= ((1 << BANG) | (1 << YOU) | (1 << HORN));
}

void initButt (void) {
  EICRA |= (1 << ISC01);      // INT0 triggers on falling edge
  EIMSK |= (1 << INT0);       // enables INT0
};

void initFuckHornTimer (void) {
  TCCR2A |= (1 << COM2A1) | (1 << COM2B1) | (1 << WGM20);   // Phase correct PWM, 0xFF is top, 8-bit
  OCR2A = 0;                  // should start as solid off
  OCR2B = 0;
  TCCR2B |= (1 << CS20);      // enables the clock with a prescaler of 1
}

void initBangYouTimer (void) {
  TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);   // Phase correct PWM, 0x00FF is top, 8-bit
  ATOMIC_BLOCK(ATOMIC_FORCEON){       // atomic operation for 16-bit register writes
    OCR1A = 0;                        // should start as solid off
    OCR1B = 0;
  }
  TCCR1B |= (1 << CS10);              // enables the clock with a prescaler of 1
}

// Calls initialization functions common to all FUnicorn demos
void initFUnicorn (void)  {

  initOutputs();

  //comment out these two if not using PWMs to dim/pulse LEDs
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

void gotoSleep(void) {
  uint8_t acsr_status = ACSR;
  ACSR = 0;
  uint8_t adcsra_status = ADCSRA;
  ADCSRA = 0;
  uint8_t timer0_status = TCCR0B;
  uint8_t timer1_status = TCCR1B;
  uint8_t timer2_status = TCCR2B;
  TCCR0B = 0;
  TCCR1B = 0;
  TCCR2B = 0;
  uint8_t watchdog_status = WDTCSR;
  WDTCSR = 0;

  //  PRR |= ((1 << PRTIM0) | (1 << PRTIM1) | (1 << PRTIM2) | (1 << PRADC) | (1 << PRTWI) | (1 << PRSPI) | (1 << PRUSART0));
  PRR |= ((1 << PRTIM0) | (1 << PRTIM1) | (1 << PRTIM2) | (1 << PRADC) | (1 << PRTWI) | (1 << PRSPI));
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  SMCR |= (1 << SE);    // enables sleep mode
  sleep_bod_disable();
  sleep_cpu();
  // goes to sleep

  // wakes from sleep
  SMCR &= ~(1 << SE);   // disables sleep mode
//  PRR &= ~((1 << PRTIM0) | (1 << PRTIM1) | (1 << PRTIM2) | (1 << PRADC) | (1 << PRTWI) | (1 << PRSPI) | (1 << PRUSART0));
  PRR &= ~((1 << PRTIM0) | (1 << PRTIM1) | (1 << PRTIM2) | (1 << PRADC) | (1 << PRTWI) | (1 << PRSPI));
  TCCR2B = timer2_status;
  TCCR1B = timer1_status;
  TCCR0B = timer0_status;
  WDTCSR = watchdog_status;
  ADCSRA = adcsra_status;
  ACSR = acsr_status;
  initOutputs();
}

///////////////////////////////////////////////////
// Examples of Blink Routines
///////////////////////////////////////////////////

void blinkDemo (void) {
  BUTTLED_ON;
  delay(500);

  uint8_t i;
  for (i = 0; i < 255; i++) {
    FUCK_PWM = i;
    delay(3);
  }
  FUCK_PWM = ON;
  delay(500);

  for (i = 0; i < 255; i++) {
    YOU_PWM = i;
    delay(3);
  }
  YOU_PWM = ON;
  delay(500);

  for (i = 0; i < 10; i++){
    BANG_PWM = ON;
    delay(100);
    BANG_PWM = OFF;
    delay(100);
  }
  BANG_PWM = ON;

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
  delay(2000);

  FUCK_OFF;
  YOU_OFF;
  BANG_OFF;
  HORN_OFF;
  BUTTLED_OFF;
}

void blinkYouFuck (void)
{
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

void blinkCrazy (void) {

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

void blinkAllOn(void) {
  BUTTLED_ON;

  uint8_t i;
  for (i = 0; i < 255; i++) {
    FUCK_PWM = i;
    YOU_PWM = i;
    BANG_PWM = i;
    HORN_PWM = i;
    delay(3);
  }
  FUCK_ON;
  YOU_ON;
  BANG_ON;
  HORN_ON;

  delay(3000);

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

void blinkFuckYou2X(void) {
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
