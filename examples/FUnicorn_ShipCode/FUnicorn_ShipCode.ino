
#include "FUnicorn.h"
#include <ADCTouch.h>

#define CAP_BUTT_PRESS 50           // lower value is more senstive to touch, default = 50
                                    // Different power supplies affect this, batteries need lower values
int ref0;

volatile uint8_t buttPress = 0;
volatile uint32_t buttTime = 0;
volatile uint8_t FUmode = CAP_AND_BUTT;

// records current time and sets buttPress to 1 to start debounce timing
ISR(INT0_vect) {
  if (buttPress == 0) {
    buttPress = 1;
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
  EIMSK &= ~(1 << INT0);        // disables INT0 to guarantee clearing buttPress

  if (buttPress && (millis() - buttTime > DEBOUNCE)) {
    buttPress = 0;              // clears buttPress to stop debounce timing
    EIMSK |= (1 << INT0);       // enables INT0
    if (BUTT_IS_PRESSED) return 1;
    else return 0;
  }

  EIMSK |= (1 << INT0);         // enables INT0
  return 0;
}

// The cycle of blink patterns
// Details for each in FUnicorn.cpp
void executeBlink (uint16_t cntr) {
  switch (cntr % 5) {    // cycles through 5 patterns
    case 0:
    blinkDemo();
    break;
    case 1:
    blinkCrazy();
    break;
    case 2:
    FuckYouFuckFuckYou();
    break;
    case 3:
    blinkFuckYou2X();
    break;
    case 4:
    blinkAllOn();
    break;
  }
}

void setup() {

  // sets up the unicorn
  initFUnicorn();

  // initializes the capacitive touch button value
  ref0 = ADCTouch.read(A0, 500);    // create reference values to account for touch offset

  // initializes the button as an interrupt source, both wakes from sleep and triggers LEDs
  initButt();
  sei();

  // pulses the horn LED once to show that it's on
  startupHornBlink();

}


////////////////////////////////////////////////////////////////////////////
// Main Loop
// - by default, the unicorn can be triggered by either Capacitive Touch or
//    by pressing a Big Red Button (separate kit)
// - if user presses and holds Big Red Button for the duration of a blinking pattern,
//    unicorn goes into Low Power Button-Only mode.  Best for battery operation.
// - Cap Touch needs constant power (USB or wall wart)
// - Low Power Button-Only mode goes to sleep after a blink pattern is executed,
//    and wakes up with another button press
////////////////////////////////////////////////////////////////////////////

void loop() {

    static uint16_t counter = 0;  // sets the initial value of counter but allows it to change with every loop

    // Behavior is determined by mode
    switch (FUmode) {

      // Uses Capacitive Touch or Big Red Button presses to cycle through blink patterns,
      // Unicorn needs constant source of power (USB or wall wart)
      case CAP_AND_BUTT: {
        int touchValue = ADCTouch.read(A0);   // no second parameter defaults to 100 samples
        touchValue -= ref0;                   // removes offset
        if ( (touchValue > CAP_BUTT_PRESS) || checkButt() ) {
          executeBlink(counter);
          counter++;
          delay(10);
          if (BUTT_IS_PRESSED) {              // checks to see if the button is (still) pressed
            FUmode = LOW_PWR_BUTT;            // changes modes
            delay(250);
            startupHornBlink();               // visual verification of mode change
            startupHornBlink();
            gotoSleep();
          }
        }
      }
      break;

      // Low Power Button mode, no Cap Touch and goes to sleep after a blink pattern is executed
      // Best for battery power
      case LOW_PWR_BUTT: {
        // checks validity of button press, when detected, executes a blink pattern
        if (checkButt()) {
          executeBlink(counter);
          counter++;
          delay(10);
          // goes to sleep here to reduce power
          gotoSleep();
          // wakes up from sleep here
        }
      }
      break;
    }
}
