/*

Welcome to the FUnicorn!
written by Carrie Sundra for Alpenglow Industries

This example sketch is for capacitive touch only activation.
The capacitive touch "button" is the unicorn itself.

When you touch the unicorn, the FUnicorn executes a blink sequence.
It cycles through 5 different sequences.
The sequences are not interruptable, you must wait for one
sequence to end before activating the next one.

*/

#include <FUnicorn.h>
#include <ADCTouch.h>

#define CAP_BUTT_PRESS 20           // lower value is more senstive to touch, default = 50
                                    // Different power supplies affect this, batteries need lower values
int capRef;

FUnicorn Fun;

void setup() {

  Fun.init();                   // sets up outputs and timers
  capRef = ADCTouch.read(A0, 500);    // create reference values to account for touch offset
  Fun.hornBlink();               // indicates ready
}

void loop() {

  static uint16_t counter = 0;

  int touchValue = ADCTouch.read(A0);   // no second parameter defaults to 100 samples
  touchValue -= capRef;                   // removes offset

  if (touchValue > CAP_BUTT_PRESS) {    // cycles through 5 blinking routines when unicorn is touched
    switch (counter % 5) {
      case 0:
      Fun.blinkDemo();
      break;
      case 1:
      Fun.blinkCrazy();
      break;
      case 2:
      Fun.blinkYouFuck();
      break;
      case 3:
      Fun.blinkFuckYou2X();
      break;
      case 4:
      Fun.blinkAllOn();
      break;
    }
    counter++;
  }
}
