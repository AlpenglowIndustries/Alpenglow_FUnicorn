
#include "FUnicorn.h"
#include <ADCTouch.h>

#define CAP_BUTT_PRESS 60

int ref0;

void setup() {

  initFUnicorn();

  ref0 = ADCTouch.read(A0, 500);    //create reference values to account for offset
}

void loop() {

  static uint16_t counter = 0;

  int touchValue = ADCTouch.read(A0);   //no second parameter defaults to 100 samples
  touchValue -= ref0;                   //remove offset

  if (touchValue > CAP_BUTT_PRESS) {
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
