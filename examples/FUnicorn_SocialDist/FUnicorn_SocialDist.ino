/*

Welcome to the FUnicorn!
written by Carrie Sundra for Alpenglow Industries

This example sketch is for an ultrasonic distance sensor, like the
US-100 or HC-SR04.

US-100:
Run in HC-SR04 mode without jumper
Power from 3.3V
Trig and Echo can be plugged directly into pins 12 and 13

HC-SR04:
Power from 5V (V USB)
Echo is a 5V signal and MUST BE reduced to 3.3V
  before plugging into the FUnicorn.
  - You can use a resistor divider and then wire it to pin 13
  - Or, since pin 13 is shared with SCK, there is built-in 5V
    protection if you plug echo directly into  pin 3 on the
    ISP connector.  (pin 3 is the outlined one at the notch)

When an object is within a certain distance (default is 6 ft),
  the FUnicorn executes a blink sequence.
It cycles through 5 different sequences.
The sequences are not interruptable, you must wait for one
  sequence to end before activating the next one.
It continuously powers and polls the ultrasonic sensor, best
  for operating from a constant USB power source.

*/

#include <FUnicorn.h>
#include <Ultrasonic.h>

#define SIXFEET 72  // a 6-ft distqnce reading on the sensor

FUnicorn Fun;  // sets up an FUnicorn "object", allowing you to use functions in
               // FUnicorn.cpp

Ultrasonic ultrasonic(12, 13);  // (trig, echo)

void setup() {
    // sets up the unicorn
    Fun.init();

    // pulses the horn LED once to show that it's on or freshly reset
    Fun.hornBlink();

    //  Serial.begin(9600);  // useful for debugging, determining distance
    //  setting
}

void loop() {
    static uint16_t counter = 0;
    int distance = ultrasonic.read(INC);
    //    Serial.print("distance = ");
    //    Serial.println(distance);

    // if distance is not zero and under six feet, blink pattern executes
    if (distance && distance < SIXFEET) {
        switch (counter % 5) {  // cycles through 5 patterns
            case 0:
                Fun.blinkDemo();
                break;
            case 1:
                Fun.blinkCrazy();
                break;
            case 2:
                Fun.FuckYouFuckFuckYou();
                break;
            case 3:
                Fun.blinkFuckYou2X();
                break;
            case 4:
                Fun.blinkAllOn();
                break;
        }
        counter++;
        delay(10);
    }
    delay(100);  // slows down loop to checking a max of 10 times per second
}
