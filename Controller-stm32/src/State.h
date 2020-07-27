//
// Created by tombr on 15/07/2019.
//

#ifndef CONTROLLER_2560_V2_STATE_H
#define CONTROLLER_2560_V2_STATE_H

#include <WString.h>
#include "types.h"
#include "main.h"

class State {
public:
    explicit State() {
        this->patchName = "Default";
    }

    unsigned char currentPatch = 0;
    unsigned char currentScene = 0;
    int currentBank = 0;
    bool tunerActive = false;
    bool muted = false;
    String patchName;
    String currentDisplay;

    void dump() {
        DPRINTF("Patch: ");
        DPRINTLN(this->currentPatch);

        DPRINTF("Scene: ");
        DPRINTLN(this->currentPatch);

        DPRINTF("Bank: ");
        DPRINTLN(this->currentBank);

        DPRINTF("Tuner Active: ");
        DPRINTLN(this->tunerActive);

        DPRINTF("Muted: ");
        DPRINTLN(this->muted);

        DPRINTF("Patch name: ");
        DPRINTLN(this->patchName);

        DPRINTF("Display text: ");
        DPRINTLN(this->currentDisplay);
    }

    patch_t getCurrentPatch(){
        return this->patches[this->currentPatch];
    }

private:
    struct patch_t patches[5] = {
            {"patch 1", 0, 0, -1, 0},
            {"patch 2", 1, 1, -1, 0},
            {"patch 3", 2, 2, -1, 0},
            {"patch 4", 3, 3, -1, 0},
            {"patch 5", 4, 4, -1, 0}
    };
};

#endif //CONTROLLER_2560_V2_STATE_H
