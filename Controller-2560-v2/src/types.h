//
// Created by tombr on 09/11/2018.
//

#ifndef CONTROLLER_2560_V2_TYPES_H
#define CONTROLLER_2560_V2_TYPES_H

struct patch_t {
    const char* name;
    unsigned char index;
    unsigned char programChange;
    char ccNum;
    unsigned char ccData;
};

struct state_t {
    unsigned char midiChannel;
    unsigned char currentPatch;
    unsigned char currentScene;
    unsigned char currentBank;
    bool tunerActive;
    bool muted;
    char* patchName;
};

#endif //CONTROLLER_2560_V2_TYPES_H
