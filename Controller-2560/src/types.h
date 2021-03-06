#ifndef TYPES_H
#define TYPES_H

#include <MIDI.h>

#define MIDI_HIGH 127
#define MIDI_LOW 0

struct patch_t {
    const char* name;
    unsigned char index;
    unsigned char program_change;
    char cc_num;
    unsigned char cc_data;
};

struct state_t {
    unsigned char midi_channel;
    unsigned char current_patch;
    unsigned char current_scene;
    unsigned char current_bank;
    bool tuner_active;
    bool muted;
    char* patch_name;
};

struct midi_message_t {
	midi::MidiType type;
    unsigned char data1;
    unsigned char data2;
};

bool operator==(const midi_message_t& lhs, const midi_message_t& rhs)
{
    return lhs.type == rhs.type &&
            lhs.data1 == rhs.data1 &&
            lhs.data2 == rhs.data2; 
}

bool operator!=(const midi_message_t& lhs, const midi_message_t& rhs)
{
    return lhs.type != rhs.type &&
            lhs.data1 != rhs.data1 &&
            lhs.data2 != rhs.data2; 
}

#endif