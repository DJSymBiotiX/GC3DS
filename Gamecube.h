#ifndef __GAMECUBE_H__
#define __GAMECUBE_H__

#include <Arduino.h>

// ======================
// Report Typedefinitions
// ======================

typedef union {
    // 8 bytes of datareport that we get from the controller
    uint8_t  whole8  [8];
    uint16_t whole16 [8 / 2];
    uint32_t whole32 [8 / 4];

    struct {
        uint8_t buttons1;
        uint8_t buttons2;
    };

    struct {
        // First data byte (Bitfields are sorted in LSB order)
        uint8_t a       : 1;
        uint8_t b       : 1;
        uint8_t x       : 1;
        uint8_t y       : 1;
        uint8_t start   : 1;
        uint8_t LOW3    : 1;
        uint8_t LOW2    : 1;
        uint8_t LOW1    : 1;

        // Second data byte
        uint8_t dleft   : 1;
        uint8_t dright  : 1;
        uint8_t ddown   : 1;
        uint8_t dup     : 1;
        uint8_t z       : 1;
        uint8_t r       : 1;
        uint8_t l       : 1;
        uint8_t HIGH1   : 1;

        // 3rd - 8th data bytes
        uint8_t xAxis   : 8;
        uint8_t yAxis   : 8;
        uint8_t cxAxis  : 8;
        uint8_t cyAxis  : 8;
        uint8_t left    : 8;
        uint8_t right   : 8;
    };
} Gamecube_data_t;

typedef union {
    // 3 Bytes of statusreport that we get from the controller
    uint8_t  whole8  [3];
    uint16_t whole16 [3 / 2];

    struct {
        // Device information
        uint16_t device;

        // Controller status (only runble is known)
        uint8_t status1 : 3;
        uint8_t rumble  : 1;
        uint8_t status2 : 4;
    };
} Gamecube_status_t;

// ==============
// Gamecube Class
// ==============

typedef enum {
    LEFT = 0,
    RIGHT,
    UP,
    DOWN
} C_Stick_Orientation;

class Gamecube_ {
    public:
        Gamecube_(void);
        bool begin(uint8_t pin);
        bool init(void);
        void end(void);

        // User read/write functions
        bool read(bool rumble = 0); // Default no rumble
        void write(void);

        // Writes the dump in a passed in array (in this case the report
        // structs)
        void translate_raw_data(uint8_t raw_dump[], void* data, uint8_t length);

        // Writes variables to the intern report
        inline void xAxis(uint8_t a)  { report.xAxis  = a; }
        inline void yAxis(uint8_t a)  { report.yAxis  = a; }
        inline void cxAxis(uint8_t a) { report.cxAxis = a; }
        inline void cyAxis(uint8_t a) { report.cyAxis = a; }

        // Functions to communicate with the controller
        // Public for debug access
        void send(uint8_t *buffer, uint8_t length, volatile uint8_t *modePort,
                volatile uint8_t *outPort, uint8_t bitMask);
        bool get(uint8_t *buffer, uint8_t length, volatile uint8_t *modePort,
                volatile uint8_t *outPort, volatile uint8_t *inPort,
                uint8_t bitMask);

        // Function to return C-Stick orientation
        C_Stick_Oriendation get_cstick_orientation();

        // Structs that store the controller states
        Gamecube_status_t status;
        Gamecube_data_t report;

    private:
        uint8_t _bitMask;
        volatile uint8_t * _outPort;
        volatile uint8_t * _inPort;
        volatile uint8_t * _modePort;
};

extern Gamecube_ Gamecube;

#endif
