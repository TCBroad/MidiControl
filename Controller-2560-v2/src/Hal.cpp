//
// Created by tombr on 09/11/2018.
//

#include "Hal.h"
#include "main.h"
#include "axefx.h"

#include <Arduino.h>
#include <Keypad.h>
#include "LiquidCrystal.h"
#include "LedControl.h"

namespace MidiController2560 {

    void Hal::init(unsigned midiChannel) {
        Serial.begin(9600);
        DPRINTLN("Starting");

        this->lcd.begin(16, 2);
        this->lcd.setCursor(0, 0);

        this->lcd.print("Starting...");

        this->midiMain.begin(midiChannel);
        this->midiAxeIn.begin(midiChannel);

        //void (Hal::*fptr)(unsigned char*, unsigned) = &Hal::handleSystemExclusive;
        this->midiAxeIn.setHandleSystemExclusive(handleSystemExclusive);

        // wake up the max7219
        this->lc.shutdown(0, false);
        this->lc.setIntensity(0, 8);
        this->lc.clearDisplay(0);
        this->lc.setChar(0, 0, '_', false);
        this->lc.setChar(0, 1, '_', false);

        digitalWrite(RED_PIN, HIGH);
        digitalWrite(GREEN_PIN, HIGH);
        digitalWrite(BLUE_PIN, HIGH);

        this->lcd.clear();

        this->lcd.setCursor(0, 0);
        this->lcd.print("Checking axe fx");
        this->midiMain.sendSysEx(6, get_firmware_version);
    }

    Hal::Hal() {
        pinMode(RED_PIN, OUTPUT);
        pinMode(GREEN_PIN, OUTPUT);
        pinMode(BLUE_PIN, OUTPUT);
        pinMode(MIDI_MESSAGE_RECEIVED_PIN, OUTPUT);
        pinMode(AXEFX_MESSAGE_RECEIVED_PIN, OUTPUT);
    }

    Keypad Hal::createKeypad() {
        byte rowPins[KEYPAD_ROWS] = {30, 32, 34, 36}; //connect to the row pinouts of the keypad
        byte colPins[KEYPAD_COLS] = {44, 46, 48, 50}; //connect to the column pinouts of the keypad
        char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
                {'0', '1', '2', '3'},
                {'4', '5', '6', '7'},
                {'8', '9', 'T', 'U'},
                {'a', 'b', 'c', 'D'}
        };

        return {(char *) keys, rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS};
    }

    void Hal::writeText(char *line1, char *line2, DisplayDuration duration) {

    }

    void Hal::updateTimers() {
        if (this->lcdCountdown > 0) {
            this->lcdCountdown--;
        }

        if (this->ledCountdown > 0) {
            this->ledCountdown--;
        }

        if (this->midiInLed > 0) {
            this->midiInLed--;
        }

        if (this->axeInLed > 0) {
            this->axeInLed--;
        }
    }

    void Hal::pollInputs() {
        this->midiAxeIn.read();
        this->midiMain.read();
        this->keypad.getKeys();
    }

    void Hal::setDigit(unsigned digit, char position) {

    }

    void Hal::sendProgramChange(byte program, unsigned channel, MidiDevice device) {
        switch (device) {
            case Main:
                this->midiMain.sendProgramChange(program, channel);
                break;
            case AxeFx:
                DPRINT("No output for this MIDI port");
                break;
        }
    }

    void Hal::sendControlChange(byte ccNumber, byte data, unsigned channel, MidiDevice device) {
        switch (device) {
            case Main:
                this->midiMain.sendControlChange(ccNumber, data, channel);
                break;
            case AxeFx:
                DPRINT("No output for this MIDI port");
                break;
        }
    }

    void Hal::sendSysEx(const unsigned char *data, int size, MidiDevice device) {
        switch (device) {
            case Main:
                this->midiMain.sendSysEx(size, data);
                break;
            case AxeFx:
                DPRINT("No output for this MIDI port");
                break;
        }
    }
}