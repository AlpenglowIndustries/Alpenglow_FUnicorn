/*

Welcome to the FUnicorn!
written by Carrie Sundra for Alpenglow Industries

This example sketch is for resistive touch activation.
Bridge the lower leg of the unicorn and the hoof to activate the blink sequence.
It helps if your finger is moist.

When you activate the resistive touch "button", the FUnicorn executes a blink
sequence. It cycles through 5 different sequences. The sequences are not
interruptable, you must wait for one sequence to end before activating the next
one.

*/

#include <FUnicorn.h>

#define RES_BUTT_PRESS \
    1015  // upper threshold for a valid resistive touch "button" press

FUnicorn Fun;  // sets up an FUnicorn "object", allowing you to use functions in
               // FUnicorn.cpp

void setup() {
    Fun.init();  // initializes the outputs and timers
                 //  Serial.begin(9600);   // uncomment serial lines and look at
                 //  output if having trouble triggering
    Fun.hornBlink();  // blinks once to indicate it's ready
}

void loop() {
    // initializes a counter used to cycle through blinking patterns
    static uint16_t counter = 0;

    // checks the value of the resistive touch leg
    int resButtValue = analogRead(A1);

    //  Serial.print("adc = ");
    //  Serial.println(resButtValue);

    if (resButtValue < RES_BUTT_PRESS) {  // determines if the leg is touched

        // cycles through blinking patterns
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

    delay(100);  // slows down code so only checks a max of 10x per second
}
