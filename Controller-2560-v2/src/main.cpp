#include <Arduino.h>
#include "MidiController.h"
#include "Hal.h"

using namespace MidiController2560;

Hal hal = Hal();
MidiController controller = MidiController(hal);

void setup() {
    controller.init();
}

void loop() {

}

void handleSystemExclusive(byte* array, unsigned size) {
    controller.processSysex(array, size);
}