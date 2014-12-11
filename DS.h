#ifndef __DS_H__
#define __DS_H__

#include <Arduino.h>

typedef enum {
    LEFT = 0,
    RIGHT,
    UP,
    DOWN
} DS_circle_pad_direction;

void button_control(char pin, char state);
void circle_pad_control(uint8_t x_val, uint8_t y_val);
void circle_pad_control_direction(DS_circle_pad_direction direction);
void initialize_ds_pins(void);

// Set the pin definitions here
class DS_ {
  public:
    static const uint8_t A       =  0;
    static const uint8_t B       =  1;
    static const uint8_t X       =  2;
    static const uint8_t Y       =  3;
    static const uint8_t L       =  4;
    static const uint8_t R       =  7;
    static const uint8_t LEFT    =  8;
    static const uint8_t RIGHT   =  9;
    static const uint8_t UP      = 10;
    static const uint8_t DOWN    = 11;
    static const uint8_t START   = 12;

    // X and Y Axis must use PWM pins
    static const uint8_t X_AXIS  =  5;
    static const uint8_t Y_AXIS  =  6;
};

extern DS_ DS;

#endif
