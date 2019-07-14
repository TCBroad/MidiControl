//
// Created by tombr on 09/11/2018.
//

#ifndef CONTROLLER_2560_V2_TYPES_H
#define CONTROLLER_2560_V2_TYPES_H

#define MIDI_HIGH 127
#define MIDI_LOW 0

#include <Arduino.h>

struct patch_t {
    const char* name;
    unsigned char index;
    unsigned char programChange;
    char ccNum;
    unsigned char ccData;
    long ledColour;
};

struct state_t {
    unsigned char midiChannel;
    unsigned char currentPatch;
    unsigned char currentScene;
    int currentBank;
    bool tunerActive;
    bool muted;
    char* patchName;
};

#endif //CONTROLLER_2560_V2_TYPES_H
