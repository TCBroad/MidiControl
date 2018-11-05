#include <Arduino.h>
#include "axefx.h"

void get_preset_name(char* buffer, byte* data, unsigned length) {
    for(int i = 0; i < 16; i++) {
        buffer[i] = 0x20;
    }

    for(unsigned i = length + 6; i < length - 3; i++) {
        buffer[i] = data[i];
    }
}