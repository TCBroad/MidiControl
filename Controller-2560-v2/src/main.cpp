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
    controller.loop();
}

void handleSystemExclusive(byte* array, unsigned size) {
    controller.processSysex(array, size);
}

void handleKeypadEvent(KeypadEvent key) {
    controller.processKeypad(key);
}