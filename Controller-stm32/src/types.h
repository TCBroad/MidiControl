//
// Created by tombr on 09/11/2018.
//

#ifndef CONTROLLER_2560_V2_TYPES_H
#define CONTROLLER_2560_V2_TYPES_H

#define MIDI_HIGH 127
#define MIDI_LOW 0

#define MAX_BANKS 5

#include <Arduino.h>

struct patch_t {
    const char* name;
    unsigned char index;
    char programChange;
    char ccNum;
    unsigned char ccData;
    long ledColour;
};

#endif //CONTROLLER_2560_V2_TYPES_H
