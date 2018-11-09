#include <Arduino.h>
#include "MidiController.h"
#include "Hal.h"

Hal hal = Hal();
MidiController controller = MidiController(hal);

void setup() {
    controller.init();
}

void loop() {

}

void handleSystemExclusive(byte* array, unsigned size) {
    hal.processSysex(array, size);
}