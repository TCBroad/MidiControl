//
// Created by tombr on 08/11/2018.
//

#include "MidiController.h"
#include "axefx.h"
#include "main.h"

#include "Hal.h"

namespace MidiController2560
{
    void MidiController::init() {
        this->hal.init(state.midiChannel);
    }

    void MidiController::processSysex(byte *data, unsigned int size) {
        if (size < 6) {
            DPRINTLN("Invalid message!");

            return;
        }

        char buffer[17];
        // const byte* data = midi_axe_in.getSysExArray();
        // unsigned char data_length = midi_axe_in.getSysExArrayLength();

        switch (data[5]) {
            case AXE_ID_FIRMWARE:
                DPRINTLN("Firmware response");

                sprintf(buffer, "Firmware: %d.%d", data[6], data[7]);
                this->hal.writeText((char *) "Axe FX connected", buffer, Medium);

                // connected to axe, get some info
                this->hal.sendSysEx(get_patch_num, 6);
                delay(50);

                break;
            case AXE_ID_PRESETNAME:
                DPRINTLN("Preset name response");
                // get_preset_name(state.patch_name, data, size);

                // initialised = true;
                break;
            case AXE_ID_PRESETNUM:
                DPRINTLN("Preset number response");
//            if (!initialised) {
//                state.current_patch = data[6];
//                midi_main.sendSysEx(7, get_scene_num);
//                delay(50);
//            }

                break;
            case AXE_ID_SCENE:
                DPRINTLN("Scene number response");
//            if (!initialised) {
//                state.current_scene = data[6] + 1; // 0 - 7
//                midi_main.sendSysEx(6, get_patch_name);
//                delay(50);
//            }

                break;
            case AXE_ID_TEMPO:
                // DPRINTLN("Tempo response");

                break;
            default:
                break;
        }
    }

    void MidiController::loop() {
        this->hal.pollInputs();
        this->hal.updateHardware(this->state, this->patches[this->state.currentPatch]);
    }

    void MidiController::processKeypad(KeypadEvent key) {
        switch (this->hal.getKeypadState()) {
            case PRESSED:
                this->handleKeyPressed(key);
                break;
            case HOLD:
                break;
            default:break;
        }
    }

    void MidiController::handleKeyPressed(KeypadEvent key) {
        // work out what it is
        if (key >= '0' && key <= '9')
        {
            auto value = key - 48; // to int from char
            DPRINTLN(value);

            this->state.tunerActive = false;

            // 0 - 4 it's a patch change
            if (value >= 0 && value <= 4)
            {
                this->state.currentPatch = static_cast<unsigned char>(value);
                struct patch_t patch = this->patches[this->state.currentPatch];

                if (patch.programChange > 0)
                {
                    this->hal.sendProgramChange(patch.programChange);
                    this->hal.sendControlChange(SCENE_CHANGE_CC, 0);

                    this->state.currentScene = 0;
                }

                if (patch.ccNum > -1)
                {
                    this->hal.sendControlChange(static_cast<byte>(patch.ccNum), patch.ccData);
                }
            }

            // 5 - 9 it's a scene change
            if (value >= 5)
            {
                this->state.currentScene = static_cast<unsigned char>(max(value - 5, 0)); // scenes are 0 - 4

//                if(keypad.getState() == HOLD) {
//                    this->state.currentScene = value; // scenes are 5 - 9
//                } else {
//                }

                this->state.tunerActive = false;

                this->hal.sendControlChange(SCENE_CHANGE_CC, this->state.currentScene);
            }
        }
        else
        {
            // command button
            byte sysex[] = { 0x00, 0x01, 0x74, 0x03, 0x08, 0x0e };
            switch (key)
            {
                case 'T':
                    // tuner
                    DPRINTLN("Tuner");
                    this->state.tunerActive = !this->state.tunerActive;

                    this->hal.sendControlChange(TUNER_CC, this->state.tunerActive ? MIDI_HIGH : MIDI_LOW);
                    break;
                case 'U':
                    // bank up
                    DPRINT("Bank up: ");
                    this->state.currentBank = min(this->state.currentBank + 1, MAX_BANKS);

                    DPRINTLN(this->state.currentBank);
                    break;
                case 'D':
                    // bank down
                    DPRINT("Bank down: ");
                    this->state.currentBank = max(this->state.currentBank - 1, 0);

                    DPRINTLN(state.currentBank);
                    break;
                case 'a':
                    // global A (mute)
                    DPRINTLN("Global A (mute)");
                    state.muted = !state.muted;

                    this->hal.sendControlChange(VOLUME_CC, state.muted ? MIDI_LOW : MIDI_HIGH);
                    break;
                case 'b':
                    // global B
                    DPRINTLN("Global B");

                    this->hal.sendSysEx(sysex, 6);

                    break;
                case 'c':
                    // global C
                    DPRINTLN("Global C");

                    break;
            }
        }
    }
}