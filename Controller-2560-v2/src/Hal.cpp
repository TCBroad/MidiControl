//
// Created by tombr on 09/11/2018.
//

#include "Hal.h"
#include "main.h"

#include <Arduino.h>
#include <Keypad.h>
#include "LiquidCrystal.h"
#include "LedControl.h"

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
}

Hal::Hal() {
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    pinMode(MIDI_MESSAGE_RECIEVED_PIN, OUTPUT);
    pinMode(AXEFX_MESSAGE_RECIEVED_PIN, OUTPUT);
}

Keypad Hal:: createKeypad() {
    byte rowPins[KEYPAD_ROWS] = { 30, 32, 34, 36 }; //connect to the row pinouts of the keypad
    byte colPins[KEYPAD_COLS] = { 44, 46, 48, 50 }; //connect to the column pinouts of the keypad
    char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
            { '0','1','2','3' },
            { '4','5','6','7' },
            { '8','9','T','U' },
            { 'a','b','c','D' }
    };

    return Keypad((char*)keys, rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);
}

void Hal::processSysex(byte *data, unsigned int size) {

}
