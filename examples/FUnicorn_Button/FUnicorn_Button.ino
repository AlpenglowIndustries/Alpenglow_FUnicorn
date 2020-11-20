
#include "FUnicorn.h"

volatile uint8_t buttPress = 0;
volatile uint32_t buttTime = 0;

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


void setup() {

  // sets up the unicorn
  initFUnicorn();

  // initializes the button as an interrupt source, both wakes from sleep and triggers LEDs
  initButt();
  sei();

  // pulses the horn LED once to show that it's on
  startupHornBlink();

  gotoSleep();

}

void loop() {

    static uint16_t counter = 0;

    if (checkButt()) {
      switch (counter % 5) {    // cycles through 5 patterns
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
      counter++;
      delay(10);
      gotoSleep();
    }
}
