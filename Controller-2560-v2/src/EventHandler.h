//
// Created by tombr on 17/07/2019.
//

#ifndef CONTROLLER_2560_V2_EVENTHANDLER_H
#define CONTROLLER_2560_V2_EVENTHANDLER_H

#include <Keypad.h>
#include "HardwareController.h"
#include "State.h"

namespace MidiController2560 {

    class EventHandler {
    public:
        explicit EventHandler(const HardwareController &controller, const State &state);

        void handleSysEx(byte* array, unsigned size);
        void handleSwitch(KeypadEvent key);

    private:
        HardwareController controller;
        State state;

        void handleKeyPressed(KeypadEvent key);

        void changePatch(unsigned char patch);

        void changeScene(unsigned char scene);
    };
}



#endif //CONTROLLER_2560_V2_EVENTHANDLER_H
