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

#define TLC_DATA 25
#define TLC_CLK 26
#define TLC_LATCH 27

#define LCD_CLEAR "                "

#include "types.h"

#include <Arduino.h>
#include <MIDI.h>
#include <Keypad.h>
#include "LiquidCrystal.h"
#include "LedControl.h"
#include "Adafruit_TLC5947.h"

namespace MidiController2560 {
    enum DisplayDuration : long {
        Blink = 1000,
        Short = 5000,
        Medium = 7000,
        Long = 10000,
        Hold = -1
    };

    class Hal {
    public:
        Hal();

        // hardware
        void init(byte midiChannel);

        void pollInputs();
        void updateHardware(struct state_t &state, struct patch_t &currentPatch);

        // display
        void writeText(char *line1, char *line2, DisplayDuration duration);

        // midi
        void sendProgramChange(byte program);

        void sendControlChange(byte ccNumber, byte data);

        void sendSysEx(const byte *data, unsigned int size);

        KeyState getKeypadState();

    private:
        LedControl lc = LedControl(LC_DATA_IN, LC_CLK, LC_LOAD, 1);
        Keypad keypad = createKeypad();

        LiquidCrystal lcd = LiquidCrystal(8, 9, 4, 5, 6, 7);
        Adafruit_TLC5947 tlc = Adafruit_TLC5947(1, TLC_CLK, TLC_DATA, TLC_LATCH);

        midi::MidiInterface<HardwareSerial> midiMain = midi::MidiInterface<HardwareSerial>((HardwareSerial &) Serial3);
        midi::MidiInterface<HardwareSerial> midiAxeIn = midi::MidiInterface<HardwareSerial>((HardwareSerial &) Serial2);

        char lastMessage[33];
        long ledCountdown = Blink;
        long lcdCountdown = Long;

        unsigned midiInLed = 100;
        unsigned axeInLed = 100;

        byte midiChannel;

        Keypad createKeypad();

        void writeText(char *line1, char *line2);

        void updateTimers();
        void updateLcd(struct state_t &state);
        void updateLeds(struct state_t &state, struct patch_t &currentPatch);
    };
}
#endif //CONTROLLER_2560_V2_HAL_H
