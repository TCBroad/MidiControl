//
// Created by tombr on 09/11/2018.
//

#ifndef CONTROLLER_2560_V2_HAL_H
#define CONTROLLER_2560_V2_HAL_H

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

#define RED_PIN 11
#define GREEN_PIN 12
#define BLUE_PIN 13

#define MIDI_MESSAGE_RECIEVED_PIN 28
#define AXEFX_MESSAGE_RECIEVED_PIN 29

#define LC_DATA_IN 24
#define LC_CLK 22
#define LC_LOAD 23

#define LCD_CLEAR "                ";

#include <string.h>

#include <Arduino.h>
#include <MIDI.h>
#include <Keypad.h>
#include "LiquidCrystal.h"
#include "LedControl.h"

class Hal {
public:
    Hal();

    void init(unsigned midiChannel);

    void processSysex(byte *data, unsigned int size);

private:
    LedControl lc = LedControl(LC_DATA_IN, LC_CLK, LC_LOAD, 1);
    Keypad keypad = createKeypad();

    LiquidCrystal lcd = LiquidCrystal(8, 9, 4, 5, 6, 7);

    midi::MidiInterface<HardwareSerial> midiMain = midi::MidiInterface<HardwareSerial>((HardwareSerial&)Serial3);
    midi::MidiInterface<HardwareSerial> midiAxeIn = midi::MidiInterface<HardwareSerial>((HardwareSerial&)Serial2);

    char* lastMessage;
    unsigned long led_countdown = 1000;
    unsigned long lcd_countdown = 10000;

    Keypad createKeypad();
};

#endif //CONTROLLER_2560_V2_HAL_H
