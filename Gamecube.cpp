#include "Gamecube.h"
//
// Gamecube Init
//

Gamecube_ Gamecube;

Gamecube_::Gamecube_(void) {
    // Prevents send/get from being called without calling begin()
    _bitMask = 0;
    // Clears the reports
    memset(&status, 0x00, sizeof(status));
    memset(&report, 0x00, sizeof(report));
}

bool Gamecube_::begin(uint8_t pin) {
    // Get the port mask and the pointers to the in/out/mode registers
    _bitMask = digitalPinToBitMask(pin);
    uint8_t port = digitalPinToPort(pin);
    _outPort = portOutputRegister(port);
    _inPort = portInputRegister(port);
    _modePort = portModeRegister(port);

    // Return status information for optional use
    return init();
}

bool Gamecube_::init(void) {
    // Only read values if begin() was called before
    if (!_bitMask) return false;

    // Initialize the gamecube conroller by sending it a null byte
    // This is unnecessary for a standard controller, but is required for the
    // Wavebird

    // 1 Received bit per byte raw input dump
    // for fast measurement

    // The dump is used for sending and getting new information in the same
    // array

    uint8_t raw_dump[sizeof(status) * 8];
    uint8_t command[] = { 0x00 };

    // We don't want interrupts getting in the way
    noInterrupts();

    // Send the command
    send(command, sizeof(command), _modePort, _outPort, _bitMask);

    // Read in data and dump it to raw_dump
    bool newinput = get(raw_dump, sizeof(raw_dump), _modePort, _outPort,
            _inPort, _bitMask);

    // End of time sensitive code
    interrupts();

    // Only translate the data if the input was valid
    if (newinput) {
        // Translate the data in raw_dump to something useful
        translate_raw_data(raw_dump, &status, sizeof(status));

        // Switch the first two bytes to compare it easy with the
        // documentation
        uint8_t temp = status.whole8[0];
        status.whole8[0] = status.whole8[1];
        status.whole8[1] = temp;
    }

    // Set c-stick orientation
    calculate_c_stick_orientation();

    // Return the status information for optional use
    return newinput;
}

void Gamecube_::end(void) {
    // Only read values if begin() was called before
    if (!_bitMask) return;

    // Turns off rumble by sending a normal reading request and discards the
    // information

    uint8_t command[] = { 0x40, 0x03, 0x00 };

    // Don't want interrupts getting in the way
    noInterrupts();

    // Send the command
    send(command, sizeof(command), _modePort, _outPort, _bitMask);

    // End of time sensitive code
    interrupts();

    // Stupid routine to wait for the gamecube controller to stop sending its
    // response. We don't care what it is, but we can't start asking for
    // status if it's still responding
    for (int i = 0; i < 10; i++) {
        // Make sure the line is idle for 10 iterations, should be plenty.
        // (About 7us)
        if (!(*_inPort & _bitMask)) {
            i = 0;
        }
    }

    // Reset that you need to call begin() again first
    _bitMask = 0;
}

//
// Gamecube Read
//

bool Gamecube_::read(bool rumble) {
    // Only read values if begin() was called before
    if (!_bitMask) return false;

    // Command to send to the gamecube controller. The last bit is rumble

    uint8_t raw_dump[sizeof(report) * 8];
    uint8_t command[] = { 0x40, 0x03, rumble & 0x01 };

    // Don't want interrupts getting in the way
    noInterrupts();

    // Send the command
    send(command, sizeof(command), _modePort, _outPort, _bitMask);

    // Read in data and dump it to raw_dump
    bool newinput = get(raw_dump, sizeof(raw_dump), _modePort, _outPort,
            _inPort, _bitMask);

    // End of time sensitive code
    interrupts();

    // Only translate the dataa if the input was valid
    if (newinput) {
        // Translate the data in raw_dump to something useful
        translate_raw_data(raw_dump, &report, sizeof(report));
    }

    // Set the c-stick orientation
    calculate_c_stick_orientation();

    // Return status information for optional use
    return newinput;
}

void Gamecube_::translate_raw_data(uint8_t raw_dump[], void *data,
        uint8_t length) {
    // The get function sloppily dumps its data 1 bit pre byte into the
    // get_status_extended char awway. It's out job to go through that and put
    // each piece neatly into the struct
    uint8_t *report = (uint8_t *)data;
    memset(report, 0, length);

    for (int i = 0; i < length * 8; i++) {
        report [i / 8] |= raw_dump[i] ? (0x80 >> (i % 8)) : 0;
    }
}

//
// Gamecube i/o Functions
//

//
// Send
//

// This sends the given byte sequence to the controller.
// Length must be at least 1.
// It destroys the buffer passed in as it send it.
// It fills the buffer with the answer from the controller
void Gamecube_::send(uint8_t *buffer, uint8_t length,
        volatile uint8_t *modePort, volatile uint8_t *outPort,
        uint8_t bitMask) {
    // Prepare pin for output and pull it high
    *outPort  |= bitMask; // HIGH
    *modePort |= bitMask; // OUT
    *outPort  |= bitMask; // HIGH

    // This routine is very carefully timed by examining the assembly output
    // Do not change any statements, it could throw the timings off
    //
    // We get 16 cycles per microsecond, which should be plenty, but we need
    // to be conservative. Most assembly ops take 1 cycle, but a few take 2
    //
    // I use manually constructed for-loops out of gotos so I have more
    // control over the outputted assembly. I can insert nops where it was
    // impossible with a for loop

    // Starting outer for loop
    outer_loop:
    {
        // Starting inner for loop
        // Send these bytes
        uint8_t bits = 8;
        inner_loop:
        {
            // Starting a bit, set the line low
            *outPort &= ~bitMask; // LOW, 5 cycles

            // Branching
            if (*buffer >> 7) { // 8 Cycles to enter
                // Bit is a 1, 2 cycles
                // remain low for 1us, then go high for 3us
                // NOP block 1
                asm volatile ("nop\n");

                // Setting line to high
                *outPort |= bitMask; // HIGH, 5 cycles

                // NOP block 2
                // We'll wait only 2 us to sync up with both conditions
                // at the bottom of the if statement
                asm volatile (
                    "nop\nnop\nnop\nnop\nnop\n"
                    "nop\nnop\nnop\nnop\nnop\n"
                    "nop\nnop\nnop\nnop\nnop\n"
                    "nop\nnop\nnop\nnop\nnop\n"
                    "nop\nnop\nnop\nnop\nnop\n"
                    "nop\nnop\nnop\nnop\nnop\n"
                );
            } else {
                // Bit is a 0, 3 cycles to enter
                // Remain low for 3us, then go high for 1 us
                // NOP block 3
                asm volatile (
                    "nop\nnop\nnop\nnop\nnop\n"
                    "nop\nnop\nnop\nnop\nnop\n"
                    "nop\nnop\nnop\nnop\nnop\n"
                    "nop\nnop\nnop\nnop\nnop\n"
                    "nop\nnop\nnop\nnop\nnop\n"
                    "nop\nnop\nnop\nnop\nnop\n"
                    "nop\nnop\n"
                );

                // Setting the line to high
                *outPort |= bitMask; // HIGH, 5 cycles

                // Wait for 1us
                // End of conditional branch, need to wait 1us more before
                // next bit
            }
            // End of the if, the line is high and needs to remain high for
            // exactly 16 more cycles, regardless of the previous branch path

            // Finishing inner loop body
            --bits;
            if (bits != 0) {
                // NOP block 4
                // This block is why a for loop was impossible
                asm volatile ("nop\nnop\n");
                // Rotating out bits
                *buffer <<= 1;

                goto inner_loop;
            } // Fall out of inner loop
        }
        // Continuing outer loop
        // In this case: The inner loop exits and the outer loop iterates,
        // there are /exactly/ 16 cycles taken up by the necessary operations.
        // So no NOPs are needed here (that was lucky!)
        // Due to the modification of the 2 unnecessary cycles somehow
        // dissapeared and this information might be wrong

        --length;
        if (length != 0) {
            ++buffer;
            // NOP block 5 (added)
            // asm volatile ("nop\n");
            goto outer_loop;
        } // fall out of outer loop
    }

    // Send a single stop (1) bit
    // NOP block 6
    asm volatile ("nop\nnop\nnop\nnop\nnop\n");
    *outPort &= ~bitMask; // LOW, 5 cycles

    // Wait 1us, 16 cycles, then raise the line
    // NOP block 7
    asm volatile (
        "nop\nnop\nnop\nnop\nnop\n"
        "nop\nnop\nnop\nnop\nnop\n"
        "nop\n"
    );
    *outPort |= bitMask; // HIGH, 5 cycles
}

bool Gamecube_::get(uint8_t *buffer, uint8_t length,
        volatile uint8_t *modePort, volatile uint8_t *outPort,
        volatile uint8_t *inPort, uint8_t bitMask) {
    // Prepare pin for input with pullup
    *modePort &= ~bitMask; // IN
    *outPort |= bitMask; // HIGH, 5 cycles

    // Listen for the expected 8 bytes of data back from the controller and
    // blast it out to the raw_dump array, one bit per byte for extra speed.
    // Afterwards, call translate_raw_data() to interpret the raw data and
    // pack it into the struct
    // Starting to listen
    uint8_t timeout;

    // Again, using gotos here to make the assembly more predictable and
    // optimization easier (please don't kill me)
    get_loop:
    {
        // The port thing takes way longer, so timeout is 5 loops, this means
        // after 4us it will timeout
        timeout = 0x05;

        // Wait for the line to go low
        while (*inPort & bitMask) {
            timeout --;
            if (!timeout) {
                return false;
            }
        }

        // Wait approx 1.5us and poll the line
        asm volatile (
            "nop\nnop\nnop\nnop\nnop\n"
            "nop\nnop\nnop\nnop\nnop\n"
            "nop\nnop\nnop\nnop\nnop\n"
            "nop\nnop\n"
        );

        *buffer = *inPort & bitMask;

        ++buffer;
        --length;
        if (length == 0) {
            return true;
        }

        // Wait for line to go high again, 1.5us
        asm volatile (
            "nop\nnop\nnop\nnop\nnop\n"
            "nop\nnop\nnop\nnop\nnop\n"
            "nop\nnop\nnop\nnop\nnop\n"
            "nop\n"
        );
        goto get_loop;
    }
}

// Gamecube Helper functions
void Gamecube_::calculate_c_stick_orientation() {
    // Reset C-Stick Object
    c_stick.whole8 = 0;

    // XXX: Fine tune these values a little more
    // Left
    if (report.cxAxis < 80) {
        c_stick.left = 1;
    }

    // Right
    if (report.cxAxis > 170) {
        c_stick.right = 1;
    }

    // Up
    if (report.cyAxis > 170) {
        c_stick.up = 1;
    }

    // Down
    if (report.cyAxis < 80) {
        c_stick.down = 1;
    }
}
