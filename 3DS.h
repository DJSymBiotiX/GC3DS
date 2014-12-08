#ifndef __3DS_H__
#define __3DS_H__

#include <Arduino.h>

void button_control(char pin, char state);
void initialize_3ds_pins(void);

// Set the pin definitions here
typedef struct {
    uint8_t A       =  0;
    uint8_t B       =  1;
    uint8_t X       =  2;
    uint8_t Y       =  3;
    uint8_t L       =  4;
    uint8_t R       =  7;
    uint8_t LEFT    =  8;
    uint8_t RIGHT   =  9;
    uint8_t UP      = 10;
    uint8_t DOWN    = 11;
    uint8_t START   = 12;

    // X and Y Axis must use PWM pins
    uint8_t X_AXIS  =  5;
    uint8_t Y_AXIS  =  6;
} 3DS_;



#endif
