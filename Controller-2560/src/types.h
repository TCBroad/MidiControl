#ifndef TYPES_H
#define TYPES_H

#include <midi_Defs.h>

struct patch_t {
    const char* name;
    unsigned char index;
    unsigned char program_change;
    char cc_num;
    unsigned char cc_data;
};

struct state_t {
    unsigned char midi_channel;
    char current_patch;
    bool tuner_active;
};

struct midi_message_t {
	unsigned char type;
    unsigned char data1;
    unsigned char data2;
};

#endif