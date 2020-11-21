
#include "FUnicorn.h"
#include <ADCTouch.h>

#define CAP_BUTT_PRESS 50           // lower value is more senstive to touch, default = 50
                                    // Different power supplies affect this, batteries need lower values
int ref0;

volatile uint8_t buttPress = 0;
volatile uint32_t buttTime = 0;
volatile uint8_t FUmode = CAP_TOUCH;

// records current time and sets buttPress to 1 to start debounce timing
ISR(INT0_vect) {
  FUmode = BIG_RED_BUTT;
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
// - behavior is determined by whether the Unicorn is in
//    Capacitive Touch or Big Red Button mode
// - default is Cap Touch mode, Big Red Button mode triggered by a button press
//    (mode is set in ISR)
// - to re-enter Cap Touch mode, press reset button or power cycle
// - upon a Cap Touch or Big Red button press, a blink routine is executed
// - Cap Touch mode needs constant power (cannot sleep)
// - Big Red Button mode goes to sleep after a blink pattern is executed
////////////////////////////////////////////////////////////////////////////

void loop() {

    static uint16_t counter = 0;  // sets the initial value of counter but allows it to change with every loop

    switch (FUmode) {

      // Uses Capacitive Touch to cycle through blink patterns
      case CAP_TOUCH: {
        int touchValue = ADCTouch.read(A0);   // no second parameter defaults to 100 samples
        touchValue -= ref0;                   // removes offset
        if (touchValue > CAP_BUTT_PRESS) {
          executeBlink(counter);
          counter++;
          delay(10);
        }
      }
      break;

      // Uses Big Red Button to cycle through blink patterns
      case BIG_RED_BUTT: {
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
