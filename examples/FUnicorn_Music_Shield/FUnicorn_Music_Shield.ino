/*

Welcome to the FUnicorn!
written by Carrie Sundra for Alpenglow Industries

This example sketch is the code the FUnicorn ships with.
You can activate the LED message by either touching the unicorn or pressing
an external button.  Pressing and holding the button for the duration of a blink
sequence puts the FUnicorn into a low power button-only mode.  Best for battery
operation, note that capacitive touch on the unicorn is disabled.  Re-enable
capacitive touch by power cycling or pressing the onboard blue reset button.

When the FUnicorn is activated, it executes a blink sequence.
It cycles through 5 different sequences.
The sequences are not interruptable, you must wait for one
sequence to end before activating the next one.

*/

#include <ADCTouch.h>
#include <FUnicorn.h>

#define CAP_BUTT_PRESS \
    20  // lower value is more senstive to touch, default = 20               \
                          // Different power supplies affect this, batteries need lower values \
                          // The unicorn is a less senstivie touch button to begin with,       \
                          // as the GND plane behind it isn't unbroken.
int capRef;

// pins used by Galactic FBUnicorn project
// we skip red in order to save pins
int greenLed = 12;
int blueLed = 13;
int greenLed2 = 5;
int blueLed2 = 6;

int musicPin = 7;
int remainingLoops = 0;

FUnicorn Fun;  // sets up an FUnicorn "object", allowing you to use functions in
               // FUnicorn.cpp

// variables used with interrupt routines must be declared as volatile
// explicit variable types are used for clarity
// uintX_t = unsigned integer = no negative values
// X = number of bits in the integer, 8 = 8 bits, = 0-255 for unsigned.
// signed value would be -128 to 127.  Asymmetric because 0 is a value.
volatile uint8_t buttJustPressed = 0;
volatile uint32_t buttTime = 0;

// The Interrupt Routine which handles external button presses.
// records current time and sets buttJustPressed to 1 to start debounce timing
ISR(INT0_vect) {
    if (buttJustPressed == 0) {
        buttJustPressed = 1;
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
    EIMSK &= ~(
        1 << INT0);  // disables INT0 to guarantee clearing buttJustPressed
                     // equivalent to detachInterrupt(digitalPinToInterrupt(2));
                     // (1 << INT0) bit-shifts a "1" into the interrupt 0
                     // position in the EIMSK register
                     // &= is shorthand for saying EIMSTK = EIMSK & ~(1 <<
                     // INT0); & ~ clears the INT0 bit (sets it to 0) in the
                     // EIMSK register without affecting other bit values

    // debounce time is up and we have not yet checked the button state
    if (buttJustPressed && (millis() - buttTime > DEBOUNCE)) {
        buttJustPressed = 0;  // clears buttJustPressed to stop debounce timing
        EIMSK |=
            (1
             << INT0);  // enables INT0, same shorthand as above except that
                        // "OR" "|" sets a bit to 1 somewhat equivalent to
                        // attachInterrupt(), re-enables it without changing
                        // behavior
                        //  (already set to falling edge in FUnicorn.cpp
                        //  initButt(), and interrupt routine is ISR(INT0_vect))

        // assumes bouncing is done, if button is still pressed, then it's a
        // button press! otherwise maybe it was bouncing when the button was
        // released, so it ignores that.
        if (BUTT_IS_PRESSED)
            return 1;
        else
            return 0;
    }

    // debounce time isn't done yet, or another button press hasn't come in yet
    EIMSK |= (1 << INT0);  // enables INT0
    return 0;
}

// Performs a blink pattern depending on number passed in as cntr
// Details for each in FUnicorn.cpp
void executeBlink(uint16_t cntr) {
    switch (cntr % 5) {  // cycles through 5 patterns
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
}

void setup() {
    // pin setup for Galactic FBUnicorn project
    pinMode(greenLed, OUTPUT);
    pinMode(blueLed, OUTPUT);
    pinMode(greenLed2, OUTPUT);
    pinMode(blueLed2, OUTPUT);

    // switch on LEDs to test
    digitalWrite(greenLed, HIGH);
    digitalWrite(blueLed, HIGH);
    digitalWrite(greenLed2, HIGH);
    digitalWrite(blueLed2, HIGH);

    pinMode(musicPin, OUTPUT);

    // sets up the unicorn
    Fun.init();

    // initializes the capacitive touch button value
    capRef = ADCTouch.read(
        A0, 500);  // create reference values to account for touch offset

    // initializes the button as an interrupt source, both wakes from sleep and
    // triggers LEDs
    Fun.initButt();
    sei();  // enables all interrupts, equivalent to interrupts();

    // pulses the horn LED once to show that it's on or freshly reset
    Fun.hornBlink();
}

////////////////////////////////////////////////////////////////////////////
// Main Loop
// - by default, the unicorn can be triggered by either Capacitive Touch or
//    by pressing a Big Red Button (separate kit)
// - if user presses and holds Big Red Button for the duration of a blinking
// pattern,
//    unicorn goes into Low Power Button-Only mode.  Best for battery operation.
//    To re-enable capacitive touch mode, cycle power to the FUnicorn.
// - Cap Touch needs constant power (USB or wall wart)
// - Low Power Button-Only mode goes to sleep after a blink pattern is executed,
//    and wakes up with another button press
////////////////////////////////////////////////////////////////////////////

void loop() {
    // static variables are persistent between loops, allows the initial
    // condition to be set for the first loop, then change with subsequent loops
    static uint16_t counter = 0;
    static uint8_t FUmode = CAP_AND_BUTT;

    // Behavior is determined by mode
    switch (FUmode) {
        // Uses Capacitive Touch or Big Red Button presses to cycle through
        // blink patterns, Unicorn needs constant source of power (USB or wall
        // wart) for Cap Touch mode
        case CAP_AND_BUTT: {
            int touchValue = ADCTouch.read(
                A0);  // no second parameter defaults to 100 samples
            touchValue -= capRef;  // removes offset
            if ((touchValue > CAP_BUTT_PRESS) ||
                checkButt()) {  // triggers on cap touch or button press

                // start the music
                digitalWrite(musicPin, HIGH);

                // keep blinking LEDs (approximately) as long as music lasts
                if (remainingLoops > 0) {
                    remainingLoops = 0;
                } else {
                    // blink and initiate LED loop countdown
                    // animations differ in length, so won't always be spot on
                    remainingLoops = 1428;
                    executeBlink(counter);
                }
                // de-bounce a little more than default
                delay(250);

                digitalWrite(musicPin, LOW);

                counter++;  // so that next pattern will execute next time
                delay(10);
                if (BUTT_IS_PRESSED) {  // checks to see if the button is
                                        // (still) pressed
                    FUmode =
                        LOW_PWR_BUTT;  // changes modes to low power button-only
                    delay(250);
                    Fun.hornBlink();  // visual verification of mode change
                    Fun.hornBlink();
                    Fun.sleep();  // goes into low power sleep mode
                                  // wakes from sleep here
                }
            }
        } break;

        // Low Power Button mode, no Cap Touch and goes to sleep after a blink
        // pattern is executed Best for battery power
        case LOW_PWR_BUTT: {
            // checks validity of button press, when detected, executes a blink
            // pattern
            if (checkButt()) {
                executeBlink(counter);
                counter++;
                delay(10);
                // goes to sleep here to reduce power
                Fun.sleep();
                // wakes up from sleep here
            }
        } break;
    }

    // blink LEDs, if there are loops remaining
    blinkLEDs();
}

void blinkLEDs() {
    int wait = 50;
    if (remainingLoops > 0) {
        // different blinking patterns based on loop count
        digitalWrite(greenLed, remainingLoops % 2);
        digitalWrite(blueLed, remainingLoops % 4);
        digitalWrite(greenLed2, remainingLoops % 4);
        digitalWrite(blueLed2, remainingLoops % 2);
        delay(wait);
        remainingLoops--;
    } else {
        digitalWrite(greenLed, LOW);
        digitalWrite(blueLed, LOW);
        digitalWrite(greenLed2, LOW);
        digitalWrite(blueLed2, LOW);
    }
}

// to re-enable capacitive touch mode, cycle power
