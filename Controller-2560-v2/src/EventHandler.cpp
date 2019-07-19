//
// Created by tombr on 17/07/2019.
//

#include "EventHandler.h"
#include "main.h"
#include "axefx.h"

namespace MidiController2560 {
    EventHandler::EventHandler(const MidiController2560::HardwareController &controller, const State &state) : controller(controller), state(state) {}

    void EventHandler::handleSysEx(byte *array, unsigned size) {

    }

    void EventHandler::handleSwitch(KeypadEvent key) {
        switch (this->controller.getKeyState()) {
            case PRESSED:
                this->handleKeyPressed(key);
                break;
            case HOLD:
                break;
            default:
                break;
        }
    }

    void EventHandler::handleKeyPressed(KeypadEvent key) {
        DPRINTF("EventHandler::handleKeyPressed: ");
        DPRINTLN(key);

        // command button
        byte sysex[] = {0x00, 0x01, 0x74, 0x03, 0x08, 0x0e};
        switch (key) {
            // 0 - 4 it's a patch change
            case '0':
                DPRINTLNF("Change patch: 0");
                this->changePatch(0);

                break;
            case '1':
                DPRINTLNF("Change patch: 1");
                this->changePatch(1);

                break;
            case '2':
                DPRINTLNF("Change patch: 2");
                this->changePatch(2);

                break;
            case '3':
                DPRINTLNF("Change patch: 3");
                this->changePatch(3);

                break;
            case '4':
                DPRINTLNF("Change patch: 4");
                this->changePatch(4);

                break;
                // 5 - 9 it's a scene change
            case '5':
                DPRINTLNF("Change scene: 1");
                this->changeScene(0);

                break;
            case '6':
                DPRINTLNF("Change scene: 2");
                this->changeScene(1);

                break;
            case '7':
                DPRINTLNF("Change scene: 3");
                this->changeScene(2);

                break;
            case '8':
                DPRINTLNF("Change scene: 4");
                this->changeScene(3);

                break;
            case '9':
                DPRINTLNF("Change scene: 5");
                this->changeScene(4);

                break;
            case 'T':
                // tuner
                DPRINTLNF("Tuner");
                this->state.tunerActive = !this->state.tunerActive;

                this->controller.sendControlChange(TUNER_CC, this->state.tunerActive ? MIDI_HIGH : MIDI_LOW);
                break;
            case 'U':
                // bank up
                DPRINTF("Bank up: ");
                this->state.currentBank = min(this->state.currentBank + 1, MAX_BANKS);

                DPRINTLN(this->state.currentBank);
                break;
            case 'D':
                // bank down
                DPRINTF("Bank down: ");
                this->state.currentBank = max(this->state.currentBank - 1, 0);

                DPRINTLN(state.currentBank);
                break;
            case 'a':
                // global A (mute)
                DPRINTLNF("Global A (mute)");
                state.muted = !state.muted;

                this->controller.sendControlChange(VOLUME_CC, state.muted ? MIDI_LOW : MIDI_HIGH);
                break;
            case 'b':
                // global B
                DPRINTLNF("Global B");
                this->controller.sendSysEx(sysex, 6);

                break;
            case 'c':
                // global C
                DPRINTLNF("Global C");

                this->state.dump();
                break;
            default:
                break;
        }

        this->controller.updateDisplay(this->state);
    }

    void EventHandler::changePatch(unsigned char patchNumber) {
        this->state.currentPatch = patchNumber;
        this->state.tunerActive = false;

        struct patch_t patch = this->state.getCurrentPatch();

        if (patch.programChange > -1) {
            this->controller.sendProgramChange(patch.programChange);
            this->controller.sendControlChange(SCENE_CHANGE_CC, 0);

            this->state.currentScene = 0;
        }

        if (patch.ccNum > -1) {
            this->controller.sendControlChange(patch.ccNum, patch.ccData);
        }
    }

    void EventHandler::changeScene(unsigned char scene) {
        this->state.currentScene = scene;
        this->state.tunerActive = false;

        this->controller.sendControlChange(SCENE_CHANGE_CC, this->state.currentScene);
    }
}
