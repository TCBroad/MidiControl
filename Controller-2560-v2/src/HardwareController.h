//
// Created by tombr on 17/07/2019.
//

#ifndef CONTROLLER_2560_V2_HARDWARECONTROLLER_H
#define CONTROLLER_2560_V2_HARDWARECONTROLLER_H

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4



#define LCD_CLEAR "                "

#include <LedControl.h>
#include <LiquidCrystal.h>
#include <Adafruit_TLC5947.h>
#include <Keypad.h>
#include <MIDI.h>
#include "State.h"

namespace MidiController2560 {
    class HardwareController {
    public:
        explicit HardwareController(unsigned char midiChannel);

        void setup();
        void loop();

        KeyState getKeyState();

        void sendProgramChange(unsigned char pc);
        void sendControlChange(byte cc, byte data);
        void sendSysEx(const byte *data, unsigned int size);

        void updateDisplay(State state);

    private:
        // midi stuff
        const unsigned char midiChannel;

        midi::MidiInterface<HardwareSerial> midiMain = midi::MidiInterface<HardwareSerial>((HardwareSerial &) Serial3);
        midi::MidiInterface<HardwareSerial> midiAxeIn = midi::MidiInterface<HardwareSerial>((HardwareSerial &) Serial2);

        // keypad
        Keypad *keypad;

        byte rowPins[KEYPAD_ROWS] = {30, 32, 34, 36}; //connect to the row pinouts of the keypad
        byte colPins[KEYPAD_COLS] = {44, 46, 48, 50}; //connect to the column pinouts of the keypad
        char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
                {'0', '1', '2', '3'},
                {'4', '5', '6', '7'},
                {'8', '9', 'T', 'U'},
                {'a', 'b', 'c', 'D'}
        };

        // display
        LedControl lc = LedControl(LC_DATA_IN, LC_CLK, LC_LOAD, 1);
        LiquidCrystal lcd = LiquidCrystal(8, 9, 4, 5, 6, 7);
        Adafruit_TLC5947 tlc = Adafruit_TLC5947(1, TLC_CLK, TLC_DATA, TLC_LATCH);
    };
}


#endif //CONTROLLER_2560_V2_HARDWARECONTROLLER_H
