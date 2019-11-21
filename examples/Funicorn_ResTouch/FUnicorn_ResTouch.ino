
#include "FUnicorn.h"

#define RES_BUTT_PRESS 1015 // upper threshold for a valid resistive touch "button" press

void setup() {

  initFUnicorn();
  Serial.begin(9600);
}

void loop() {

  static uint16_t counter = 0;

  int resButtValue = analogRead(A1);

  Serial.print("adc = ");
  Serial.println(resButtValue);

  if (resButtValue < RES_BUTT_PRESS) {
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

  delay(100);
}
