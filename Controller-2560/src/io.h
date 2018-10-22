#ifndef IO_H
#define IO_H

#define MAX_BANKS 10

const unsigned char rows = 4; //four rows
const unsigned char cols = 4; //three columns
unsigned char rowPins[rows] = { 30, 32, 34, 36 }; //connect to the row pinouts of the keypad
unsigned char colPins[cols] = { 44, 46, 48, 50 }; //connect to the column pinouts of the keypad
char keys[rows][cols] = {
    { '0','1','2','3' },
    { '4','5','6','7' },
    { '8','9','T','U' },
    { 'a','b','c','D' }
};

// unsigned char latch_pin = 12;
// unsigned char clock_pin = 13;
// unsigned char data_pin = 11;

const char RED_PIN = 11;
const char GREEN_PIN = 12;
const char BLUE_PIN = 13;

const char MIDI_MESSAGE_RECIEVED_PIN = 24;

#endif