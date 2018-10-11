#include <Arduino.h>
#include <MIDI.h>
#include <Keypad.h>
#include "LiquidCrystal.h"

#include "main.h"
#include "types.h"
#include "io.h"

struct patch_t patches[4] = {
    { "patch 1", 0, 1, -1, 0 },
    { "patch 2", 1, 2, -1, 0 },
    { "patch 3", 2, 3, -1, 0 },
    { "patch 4", 3, 0, 33, 3 }
};

struct state_t state = {
    2, 0, false
};

struct midi_message_t last_message;

const char* clear = "                ";
char last_display[17];

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, midi_main);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, midi_axe_in);

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {
    strcpy(last_display, clear);

    Serial.begin(9600);
    DPRINTLN("Starting");

    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
        
    lcd.print("Starting...");

    pinMode(latch_pin, OUTPUT);
    pinMode(data_pin, OUTPUT);  
    pinMode(clock_pin, OUTPUT);

    midi_main.begin(state.midi_channel);
    midi_axe_in.begin(state.midi_channel);

    lcd.clear();
}

void loop() {
    // read input
    char key_pressed = keypad.getKey();
    if (key_pressed != NO_KEY) {
        // work out what it is
        if(key_pressed >= '0' && key_pressed <= '9') {
            DPRINTLN(key_pressed);

            // it's a patch change
            state.current_patch = key_pressed - 48; // to int from char
            state.tuner_active = false;
            struct patch_t patch = patches[state.current_patch];

            if(patch.program_change > 0) {
                midi_main.sendProgramChange(patch.program_change, state.midi_channel);
            }

            if(patch.cc_num > -1) {
                midi_main.sendControlChange(patch.cc_num, patch.cc_data, state.midi_channel);
            }
        } else {
            // command button
            switch(key_pressed) {
                case 'T':
                    // tuner
                    DPRINTLN("Tuner");
                    state.tuner_active = !state.tuner_active;

                    midi_main.sendControlChange(15, state.tuner_active ? 127 : 0, state.midi_channel);
                    break;
                case 'U':
                    // bank up
                    DPRINTLN("Bank up");

                    break;
                case 'D':
                    // bank down
                    DPRINTLN("Bank down");

                    break;                
                case 'a':
                    // global A
                    DPRINTLN("Global A");

                    break;
                case 'b':
                    // global B
                    DPRINTLN("Global B");

                    break;
                case 'c':
                    // global C
                    DPRINTLN("Global C");

                    break;        
            }
        }
    }

    // read axe fx midi
    if(midi_axe_in.read()) {
        if(midi_axe_in.getChannel() == state.midi_channel) {
            last_message = {
                midi_axe_in.getType(),
                midi_axe_in.getData1(),
                midi_axe_in.getData2()
            };
        }
    }

    // update display
    update_display();
    update_leds();
}

void update_display() {
    char* output = (char*)malloc(sizeof(char) * 16);

    if(state.tuner_active) {
        sprintf(output, "Tuner active");
    } else {
        sprintf(output, "Patch: %d", state.current_patch);
    }

    if(strcmp(last_display, output) != 0) {
        DPRINT("Updating display: ");
        DPRINTLN(output);

        clear_display();

        // char* line1 = (char*)malloc(sizeof(char) * 16);
        // char* line2 = (char*)malloc(sizeof(char) * 16);
        // strncpy(line1, output, 16);
        // strncpy(line2, output + 16, 16);
        
        lcd.setCursor(0, 0);
        lcd.print(output);

        // lcd.setCursor(0, 1);
        // lcd.print(line2);

        strcpy(last_display, output);
    }

    free(output);  
}

void clear_display() {
    lcd.setCursor(0, 0);
    lcd.print(clear);
    lcd.setCursor(0, 1);
    lcd.print(clear);
}

void update_leds() { 
    byte output = 0;
    bitSet(output, state.current_patch + 1);

    digitalWrite(latch_pin, LOW);
    shiftOut(data_pin, clock_pin, MSBFIRST, output);
    digitalWrite(latch_pin, HIGH);
}