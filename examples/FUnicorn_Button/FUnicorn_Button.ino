
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

void FuckYouFuckFuckYou (void)
{
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



void setup() {

  // sets up the unicorn
  initFUnicorn();

  // initializes the button as an interrupt source, both wakes from sleep and triggers LEDs
  initButt();
  sei();

  // pulses the horn LED once to show that it's on
  startupHornBlink();

}

void loop() {

    static uint16_t counter = 0;

    // goes to sleep here
    gotoSleep();
    // wakes from sleep here when button is pressed

    // keeps running checkButt for 1 second after waking up,
    // checkButt returns a valid button press after debounce time has elapsed
    while (millis() - buttTime < 1000) {
      if (checkButt()) {
        switch (counter % 5) {    // cycles through 5 patterns
          case 0:
          FuckYouFuckFuckYou();
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
