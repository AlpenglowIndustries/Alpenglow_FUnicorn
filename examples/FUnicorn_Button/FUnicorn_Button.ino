/*

Welcome to the FUnicorn!
written by Carrie Sundra for Alpenglow Industries

This example sketch is for button-only activation, and uses a
low-power sleep mode for battery operation.

When you press the button, the FUnicorn executes a blink sequence.
It cycles through 5 different sequences.
The sequences are not interruptable, you must wait for one
sequence to end before activating the next one.

*/

#include <FUnicorn.h>

FUnicorn Fun; // sets up an FUnicorn "object", allowing you to use functions in FUnicorn.cpp

// variables used with interrupt routines must be declared as volatile
// explicit variable types are used for clarity
// uintX_t = unsigned integer = no negative values
  // X = number of bits in the integer, 8 = 8 bits, = 0-255 for unsigned.
  // signed value would be int8_t, = -128 to 127.  Asymmetric because 0 is a value.
volatile uint8_t buttJustPressed = 0;
volatile uint32_t buttTime = 0;

// The Interrupt Routine which handles external button presses.
// records current time and sets buttJustPressed to 1 to start debounce timing
ISR(INT0_vect) {
  if (buttJustPressed == 0) {
    buttJustPressed = 1;
    buttTime = millis();
  }
}

////////////////////////////////////////////////////////////////////////////
// Button Handling
// - it's a heavy-handed stupid yet simple debounce routine
// - ignores all spurious bounces under the debounce time
// - when time is up, also checks to see if button is still pressed
//    (so it won't falsely trigger on button release bounces)
// - returns 1 for a valid button press, otherwise 0
////////////////////////////////////////////////////////////////////////////
uint8_t checkButt() {
  EIMSK &= ~(1 << INT0);        // disables INT0 to guarantee clearing buttJustPressed
                                // equivalent to detachInterrupt(digitalPinToInterrupt(2));
                                // (1 << INT0) bit-shifts a "1" into the interrupt 0 position in the EIMSK register
                                // &= is shorthand for saying EIMSTK = EIMSK & ~(1 << INT0);
                                // & ~ clears the INT0 bit (sets it to 0) in the EIMSK register without affecting other bit values

  // debounce time is up and we have not yet checked the button state
  if (buttJustPressed && (millis() - buttTime > DEBOUNCE)) {
    buttJustPressed = 0;        // clears buttJustPressed to stop debounce timing
    EIMSK |= (1 << INT0);       // enables INT0, same shorthand as above except that "OR" "|" sets a bit to 1
                                // somewhat equivalent to attachInterrupt(), re-enables it without changing behavior (already set to falling edge)

    // assumes bouncing is done, if button is still pressed, then it's a button press!
    // otherwise maybe it was bouncing when the button was released, so it ignores that.
    if (BUTT_IS_PRESSED) return 1;
    else return 0;
  }

  // debounce time isn't done yet, or another button press hasn't come in yet
  EIMSK |= (1 << INT0);         // enables INT0
  return 0;
}


void setup() {

  // sets up the default unicorn inputs and outputs and timers
  Fun.init();

  // initializes the button as an interrupt source, both wakes from sleep and triggers LEDs
  Fun.initButt();
  sei();  // enables all interrupts, equivalent to interrupts();

  // pulses the horn LED once to show that it's on, or has freshly reset
  Fun.hornBlink();

  // puts the unicorn to sleep until the button is pressed
  Fun.sleep();
  // wakes up from first button press here

}

void loop() {

    // initializes a counter variable, used to cycle through blinking patterns
    static uint16_t counter = 0;

    // checks validity of button press, when detected, executes a blink pattern
    // equivalent to if (checkButt() == 1)
    if (checkButt()) {
      switch (counter % 5) {    // cycles through 5 patterns
        case 0:
        Fun.blinkDemo();
        break;
        case 1:
        Fun.blinkCrazy();
        break;
        case 2:
        Fun.FuckYouFuckFuckYou();
        break;
        case 3:
        Fun.blinkFuckYou2X();
        break;
        case 4:
        Fun.blinkAllOn();
        break;
      }
      counter++;
      delay(10);

      // goes to sleep after executing a blink pattern
      Fun.sleep();
      // wakes up from sleep here
    }
}
