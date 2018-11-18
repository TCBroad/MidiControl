//
// Created by tombr on 08/11/2018.
//

#ifndef CONTROLLER_2560_V2_MIDICONTROLLER_H
#define CONTROLLER_2560_V2_MIDICONTROLLER_H


#include "Hal.h"
#include "types.h"

namespace MidiController2560 {
    class MidiController {
    public:
        explicit MidiController(Hal hal) : hal{hal} {};

        void init();

        void loop();

        void processSysex(byte *data, unsigned int size);

        void processKeypad(KeypadEvent key);
    private:
        Hal hal;

        struct state_t state = {2, 0, 1, 0, false, false, (char *) "                "};
        // move to eeprom
        struct patch_t patches[5] = {
                {"patch 1", 0, 0, -1, 0},
                {"patch 2", 1, 1, -1, 0},
                {"patch 3", 2, 2, -1, 0},
                {"patch 4", 3, 3, -1, 0},
                {"patch 5", 4, 4, -1, 0}
        };
    };
}
#endif //CONTROLLER_2560_V2_MIDICONTROLLER_H
