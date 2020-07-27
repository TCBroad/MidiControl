//
// Created by tombr on 17/07/2019.
//

#include "HardwareController.h"
#include "main.h"
#include "axefx.h"
#include "State.h"
#include "io_stm32.h"

namespace MidiControllerStm32 {
    HardwareController::HardwareController(unsigned char midiChannel) : midiChannel(midiChannel) {
        pinMode(RED_PIN, OUTPUT);
        pinMode(GREEN_PIN, OUTPUT);
        pinMode(BLUE_PIN, OUTPUT);
        pinMode(MIDI_MESSAGE_RECEIVED_PIN, OUTPUT);
        pinMode(AXEFX_MESSAGE_RECEIVED_PIN, OUTPUT);

        this->keypad = new Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);
    }

    void HardwareController::setup() {
        DPRINTLNF("HardwareController::setup");

        this->lcd.begin(16, 2);
        this->lcd.setCursor(0, 0);

        this->lcd.print(F("Starting..."));

        this->midiMain.begin(this->midiChannel);
        this->midiAxeIn.begin(this->midiChannel);
        this->midiAxeIn.setHandleSystemExclusive(handleSystemExclusive);
        this->keypad->addEventListener(handleKeypadEvent);

        // wake up the max7219
        this->lc.begin(LC_CS);
        this->lc.shutdown(0, false);
        this->lc.setIntensity(0, 8);
        this->lc.clearDisplay(0);
        this->lc.setChar(0, 0, '_', false);
        this->lc.setChar(0, 1, '_', false);

        this->tlc.begin();

        digitalWrite(RED_PIN, HIGH);
        digitalWrite(GREEN_PIN, HIGH);
        digitalWrite(BLUE_PIN, HIGH);

        this->lcd.clear();

        this->lcd.setCursor(0, 0);
        this->lcd.print(F("Checking axe fx"));
        this->midiMain.sendSysEx(6, get_firmware_version);
    }

    void HardwareController::loop() {
        this->midiAxeIn.read();
        this->midiMain.read();
        this->keypad->getKey();
    }

    KeyState HardwareController::getKeyState() {
        return this->keypad->getState();
    }

    void HardwareController::sendProgramChange(unsigned char pc) {
        this->midiMain.sendProgramChange(pc, this->midiChannel);
    }

    void HardwareController::sendControlChange(byte cc, byte data) {
        this->midiMain.sendControlChange(cc, data, this->midiChannel);
    }

    void HardwareController::sendSysEx(const byte *data, unsigned int size) {
        this->midiMain.sendSysEx(size, data);
    }

    void HardwareController::updateDisplay(State state) {
        if (state.tunerActive) {
            this->lc.setChar(0, 0, '-', false);
            this->lc.setChar(0, 1, '-', false);
        } else {
            this->lc.setDigit(0, 0, state.currentBank, false);
            this->lc.setDigit(0, 1, state.currentPatch, false);
        }
    }
}