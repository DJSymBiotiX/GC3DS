#include "DS.h"

DS_ DS;

// Pull a pin to ground to press it
void button_control(char pin, char state) {
    if (pin < 0) {
        return;
    }

    if (state == 0) {
        pinMode(pin, INPUT);
    } else {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }
}

// Write analog values to c-stick
void circle_pad_control(uint8_t x_val, uint8_t y_val) {
    analogWrite(DS.X_AXIS, (int)(255 - x_val));
    analogWrite(DS.Y_AXIS, (int)(y_val));
}

// Write one of the 4 cardinal directions to c-stick
// XXX: Update to 8 cardinal directions?
void circle_pad_control_direction(DS_circle_pad_direction direction) {
    switch(direction) {
        case RIGHT:
            circle_pad_control(255, 127);
            break;
        case LEFT:
            circle_pad_control(0, 127);
            break;
        case UP:
            circle_pad_control(127, 255);
            break;
        case DOWN:
            circle_pad_control(127, 0);
            break;
    }
}

void initialize_ds_pins(void) {
    const uint8_t NUM_PINS = 11;
    uint8_t pin_map[NUM_PINS] = {
        DS.A, DS.B, DS.X, DS.Y, DS.L, DS.R,
        DS.LEFT, DS.RIGHT, DS.UP, DS.DOWN, DS.START
    };

    // Set up all button pins, and give them initial values
    for (int i = 0; i < NUM_PINS; i++) {
        pinMode(pin_map[i], INPUT);
        button_control(pin_map[i], 0);
    }

    // Set up c-stick pins
    pinMode(DS.X_AXIS, OUTPUT);
    pinMode(DS.Y_AXIS, OUTPUT);
    analogWrite(DS.X_AXIS, 30);
    analogWrite(DS.Y_AXIS, 200);
}
