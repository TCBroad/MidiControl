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

        char *buffer = (char *) malloc(sizeof(char) * 16);
        // const byte* data = midi_axe_in.getSysExArray();
        // unsigned char data_length = midi_axe_in.getSysExArrayLength();

        switch (data[5]) {
            case AXE_ID_FIRMWARE:
                DPRINTLN("Firmware response");

                sprintf(buffer, "Firmware: %d.%d", data[6], data[7]);
                this->hal.writeText((char *) "Axe FX connected", buffer, Medium);

                // connected to axe, get some info
                this->hal.sendSysEx(get_patch_num, 6, Main);
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
        }
    }

    void MidiController::loop() {
        this->hal.pollInputs();
        // this->hal.updateHardware(this->state, );
    }

    void MidiController::processKeypad(KeypadEvent key) {

    }
}