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
    };
}
#endif //CONTROLLER_2560_V2_MIDICONTROLLER_H
