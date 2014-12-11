#include "Gamecube.h"
#include "DS.h"
#include "stdlib.h"

// Set debug to 1 to enable it, else set to 0
#define debug 1

// Define the gamecube data pin
#define GC_DATA_PIN 13

void setup() {
    if (debug) {
        // Set up Serial Communications
        Serial.begin(9600);
    }

    // Switching timer0 presclar to 1 so that PWM frequency on pin 5 and 6 is
    // 62.5KHz, making it easier to filter.
    // For details see http://playground.arduino.cc/Main/TimerPWMCheatsheet
    TCCR0B = (TCCR0B & 0b11111000) | 0x01;

    // Init 3DS Pins
    initialize_ds_pins();

    // Wait for GC controller (set up polling instead to detect if a
    // controller is connected or not?) [Rumble when found :)]
    // while(Gamecube.find_controller(GC_DATA_PIN) == false);
    while(Gamecube.begin(GC_DATA_PIN) == false);
}

void loop() {
    // Read data from the GB controller into the report struct
    Gamecube.read(false);

    /*
    // Trigger 3DS buttons from GC Buttons
    // XXX: Add mapping from GC to 3DS so you can code in any mapping
    // button_control(3DS.A, Gamecube.report.a);
    // Don't trigger A until we check for c-stick smashes
    button_control(DS.B, Gamecube.report.b);
    button_control(DS.X, Gamecube.report.x);
    button_control(DS.Y, Gamecube.report.y);
    button_control(DS.LEFT, Gamecube.report.dleft);
    button_control(DS.RIGHT, Gamecube.report.dright);
    button_control(DS.UP, Gamecube.report.dup);
    button_control(DS.DOWN, Gamecube.report.ddown);
    button_control(DS.START, Gamecube.report.start);

    // L/R Triggers
    // Do we want to trigger L or R on the triggers or the buttons?
    // Lets assume buttons for now
    button_control(DS.L, Gamecube.report.l);
    button_control(DS.R, Gamecube.report.r);

    // Code for triggers. Trigger after half way down
    // (can we just use the bool expression here?)
    //button_control(DS.L, Gamecube.report.left > 127 ? 1 : 0);
    //button_control(DS.R, Gamecube.report.right > 127 ? 1 : 0);

    // Trigger 3DS-Stick X and Y Axis from GC Stick (Not C-Pad)
    // Don't trigger circle pad until we check for c-stick smashes
    // circle_pad_control(Gamecube.report.xAxis, Gamecube.report.yAxis);

    // Smash Attacks (Is there even diagonal support for this?)
    if (Gamecube.c_stick.left) {
        circle_pad_control_direction(LEFT);
        button_control(DS.A, true);
    } else if (Gamecube.c_stick.right) {
        circle_pad_control_direction(RIGHT);
        button_control(DS.A, true);
    } else if (Gamecube.c_stick.up) {
        circle_pad_control_direction(UP);
        button_control(DS.A, true);
    } else if (Gamecube.c_stick.down) {
        circle_pad_control_direction(DOWN);
        button_control(DS.A, true);
    } else {
        // If the c-stick isn't touched, then just report A as usual
        // and also report the circle pad as usual as well
        button_control(DS.A, Gamecube.report.a);
        circle_pad_control(Gamecube.report.xAxis, Gamecube.report.yAxis);
    }
    */
}
