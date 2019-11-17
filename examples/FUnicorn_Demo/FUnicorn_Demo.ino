
#include "FUnicorn.h"

volatile uint8_t buttPress = 0;
volatile uint32_t buttTime = 0;

ISR(INT0_vect) {
  buttTime = millis();
  buttPress = 1;
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
  EIMSK &= ~(1 << INT0);        // disables INT0 to guarantee clearing buttIsPressed
  if (buttPress && (millis() - buttTime > DEBOUNCE)) {
    buttPress = 0;
    EIMSK |= (1 << INT0);       // enables INT0
    if (BUTT_IS_PRESSED) return 1;
    else return 0;
  }
  EIMSK |= (1 << INT0);         // enables INT0
  return 0;
}

void setup() {

  initFUnicorn();

  // initializes the button as an interrupt source, both wakes from sleep and triggers LEDs
  initButt();
  sei();

}

void loop() {

    static uint16_t counter = 0;

    gotoSleep();
    while (millis() - buttTime < 1000) {
      if (checkButt()) {
        switch (counter % 5) {
          case 0:
          blinkDemo();
          break;
          case 1:
          blinkCrazy();
          break;
          case 2:
          blinkYouFuck();
          break;
          case 3:
          blinkFuckYou2X();
          break;
          case 4:
          blinkAllOn();
          break;
        }
        counter++;
      }
      delay(10);
    }
}
