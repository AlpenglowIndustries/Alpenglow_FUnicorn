
#include "FUnicorn.h"
#include <ADCTouch.h>

#define CAP_BUTT_PRESS 50           // lower value is more senstive to touch, default = 50
                                    // Different power supplies affect this, batteries need lower values
int ref0;

void setup() {

  initFUnicorn();                   // sets up outputs and timers
  ref0 = ADCTouch.read(A0, 500);    // create reference values to account for touch offset
  startupHornBlink();               // indicates ready
}

void loop() {

  static uint16_t counter = 0;

  int touchValue = ADCTouch.read(A0);   // no second parameter defaults to 100 samples
  touchValue -= ref0;                   // removes offset

  if (touchValue > CAP_BUTT_PRESS) {    // cycles through 5 blinking routines when unicorn is touched
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
}
