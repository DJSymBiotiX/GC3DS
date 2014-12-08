#include "Gamecube.h"
#include "3DS.h"
#include "stdlib.h"

// Set debug to 1 to enable it, else set to 0
#define debug 1

void setup() {
    if (debug) {
        // Set up Serial Communications
        Serial.begin(9600);
    }

    // Init 3DS Pins
    initialize_3ds_pins();

    // Switching timer0 presclar to 1 so that PWM frequency on pin 5 and 6 is
    // 62.5KHz, making it easier to filter.
    // For details see http://playground.arduino.cc/Main/TimerPWMCheatsheet
    TCCR0B = TCCR0B & 0b11111000 | 0x01;
}


