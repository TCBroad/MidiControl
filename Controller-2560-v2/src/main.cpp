#include "main.h"
#include <Keypad.h>

#include "HardwareController.h"
#include "State.h"
#include "EventHandler.h"

using namespace MidiController2560;

State state;
HardwareController controller(2);
EventHandler eventHandler(controller, state);

void setup() {
    Serial.begin(9600);
    DPRINTLNF("Starting");

    controller.setup();
}

void loop() {
    controller.loop();
}

void handleKeypadEvent(KeypadEvent key) {
    eventHandler.handleSwitch(key);
}

void handleSystemExclusive(byte *array, unsigned size) {
    eventHandler.handleSysEx(array, size);
}