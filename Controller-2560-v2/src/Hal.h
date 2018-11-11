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

#define MIDI_MESSAGE_RECEIVED_PIN 28
#define AXEFX_MESSAGE_RECEIVED_PIN 29

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

namespace MidiController2560 {
    enum DisplayDuration : long {
        Blink = 1000,
        Short = 5000,
        Medium = 7000,
        Long = 10000,
        Hold = -1
    };

    enum MidiDevice {
        Main,
        AxeFx
    };

    class Hal {
    public:
        Hal();

        // hardware
        void init(unsigned midiChannel);

        void pollInputs();

        // display
        void writeText(char *line1, char *line2, DisplayDuration duration);

        void setDigit(unsigned digit, char position);

        // midi
        void sendProgramChange(byte program, unsigned channel, MidiDevice device);

        void sendControlChange(byte ccNumber, byte data, unsigned channel, MidiDevice device);

        void sendSysEx(const unsigned char *data, int size, MidiDevice device);

    private:
        LedControl lc = LedControl(LC_DATA_IN, LC_CLK, LC_LOAD, 1);
        Keypad keypad = createKeypad();

        LiquidCrystal lcd = LiquidCrystal(8, 9, 4, 5, 6, 7);

        midi::MidiInterface<HardwareSerial> midiMain = midi::MidiInterface<HardwareSerial>((HardwareSerial &) Serial3);
        midi::MidiInterface<HardwareSerial> midiAxeIn = midi::MidiInterface<HardwareSerial>((HardwareSerial &) Serial2);

        char *lastMessage;
        long ledCountdown = Blink;
        long lcdCountdown = Long;

        unsigned midiInLed = 100;
        unsigned axeInLed = 100;

        Keypad createKeypad();

        void updateTimers();
    };
}
#endif //CONTROLLER_2560_V2_HAL_H
