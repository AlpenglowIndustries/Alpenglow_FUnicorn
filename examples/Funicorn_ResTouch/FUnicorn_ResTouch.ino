/*

Welcome to the FUnicorn!
written by Carrie Sundra for Alpenglow Industries

This example sketch is for resistive touch activation.
Bridge the lower leg of the unicorn and the hoof to activate the blink sequence.
It helps if your finger is moist.

When you activate the resistive touch "button", the FUnicorn executes a blink sequence.
It cycles through 5 different sequences.
The sequences are not interruptable, you must wait for one
sequence to end before activating the next one.

*/

#include <FUnicorn.h>

#define RES_BUTT_PRESS 1015 // upper threshold for a valid resistive touch "button" press

FUnicorn Fun;

void setup() {

  Fun.init();
//  Serial.begin(9600);   // uncomment serial lines and look at output if having trouble triggering
  Fun.hornBlink();
}

void loop() {

  static uint16_t counter = 0;

  int resButtValue = analogRead(A1);

//  Serial.print("adc = ");
//  Serial.println(resButtValue);

  if (resButtValue < RES_BUTT_PRESS) {
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

  delay(100);
}
