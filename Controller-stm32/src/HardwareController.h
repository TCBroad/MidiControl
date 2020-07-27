//
// Created by tombr on 17/07/2019.
//

#ifndef CONTROLLER_2560_V2_HARDWARECONTROLLER_H
#define CONTROLLER_2560_V2_HARDWARECONTROLLER_H

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

#define LCD_CLEAR "                "

#include "../lib/ShiftedLCD/ShiftedLCD.h"
#include "../lib/tlc5947/TLC5947_SPI.h"
#include <LedControl_HW_SPI.h>
#include <Keypad.h>
#include <MIDI.h>
#include "State.h"

#include "io_stm32.h"

namespace MidiControllerStm32 {
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
        LedControl_HW_SPI lc = LedControl_HW_SPI();
        LiquidCrystal lcd = LiquidCrystal(LCD_CS);
        TLC5947_SPI tlc = TLC5947_SPI(1, TLC_CS);
    };
}


#endif //CONTROLLER_2560_V2_HARDWARECONTROLLER_H
