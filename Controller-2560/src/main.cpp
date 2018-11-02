#include <Arduino.h>
#include <MIDI.h>
#include <Keypad.h>
#include "LiquidCrystal.h"
#include "LedControl.h"

#include "main.h"
#include "types.h"
#include "io.h"
#include "axefx.h"

// move to eeprom
struct patch_t patches[5] = {
    {"patch 1", 0, 1, -1, 0},
    {"patch 2", 1, 2, -1, 0},
    {"patch 3", 2, 3, -1, 0},
    {"patch 4", 3, 4, -1, 0},
    {"patch 5", 4, 5, -1, 0}
};

// channel, patch, scene, bank, tuner
struct state_t state = {
    2, 0, 1, 0, false, false
};

// global vars
struct midi_message_t last_axe_message;
struct midi_message_t last_main_message;

const char *clear = "                ";
char last_display1[32];
char last_display2[32];

long keypad_hold_timer;

unsigned char midi_main_countdown = 0;
unsigned char midi_axe_countdown = 0;

// Hardware
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, midi_main);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, midi_axe_in);

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
LedControl lc = LedControl(LC_DATA_IN, LC_CLK, LC_LOAD, 1);

void setup()
{
    strcpy(last_display1, clear);
    strcpy(last_display2, clear);

    Serial.begin(9600);
    DPRINTLN("Starting");

    lcd.begin(16, 2);
    lcd.setCursor(0, 0);

    lcd.print("Starting...");

    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    pinMode(MIDI_MESSAGE_RECIEVED_PIN, OUTPUT);
    pinMode(AXEFX_MESSAGE_RECIEVED_PIN, OUTPUT);

    keypad.setHoldTime(500);

    // wake up the max7219
    lc.shutdown(0,false);
    lc.setIntensity(0,8);
    lc.clearDisplay(0);

    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, HIGH);

    midi_main.begin(state.midi_channel);
    midi_axe_in.begin(state.midi_channel);

    lcd.clear();
}

void loop()
{
    handle_input();
    handle_midi_in();
    handle_axe_fx();
    update_display();
    update_leds();
    update_state();
}

void handle_input() {
    digitalWrite(MIDI_MESSAGE_RECIEVED_PIN, LOW);
    digitalWrite(AXEFX_MESSAGE_RECIEVED_PIN, LOW);

    char key_pressed = keypad.getKey();
    if (key_pressed != NO_KEY)
    {
        // work out what it is
        if (key_pressed >= '0' && key_pressed <= '9')
        {
            int value = key_pressed - 48; // to int from char
            DPRINTLN(value);

            state.tuner_active = false;

            // 0 - 4 it's a patch change
            if (value >= 0 && value <= 4)
            {
                state.current_patch = value;
                struct patch_t patch = patches[state.current_patch];

                if (patch.program_change > 0)
                {
                    midi_main.sendProgramChange(patch.program_change, state.midi_channel);
                    midi_main.sendControlChange(SCENE_CHANGE_CC, 1, state.midi_channel);

                    state.current_scene = 1;
                }

                if (patch.cc_num > -1)
                {
                    midi_main.sendControlChange(patch.cc_num, patch.cc_data, state.midi_channel);
                }
            }

            // 5 - 9 it's a scene change
            if (value >= 5)
            {
                if(keypad.getState() == HOLD) {                    
                    state.current_scene = value + 1; // scenes are 6 - 10                    
                } else {
                    state.current_scene = value - 4; // scenes are 1 - 5
                }

                state.tuner_active = false;

                midi_main.sendControlChange(SCENE_CHANGE_CC, state.current_scene, state.midi_channel);
            }
        }
        else
        {
            // command button
            switch (key_pressed)
            {
                case 'T':
                    // tuner
                    DPRINTLN("Tuner");
                    state.tuner_active = !state.tuner_active;

                    midi_main.sendControlChange(TUNER_CC, state.tuner_active ? MIDI_HIGH : MIDI_LOW, state.midi_channel);
                    break;
                case 'U':
                    // bank up
                    DPRINT("Bank up: ");
                    state.current_bank = min(state.current_bank + 1, MAX_BANKS);

                    DPRINTLN(state.current_bank);
                    break;
                case 'D':
                    // bank down
                    DPRINT("Bank down: ");
                    state.current_bank = max(state.current_bank - 1, 0);

                    DPRINTLN(state.current_bank);
                    break;
                case 'a':
                    // global A (mute)
                    DPRINTLN("Global A (mute)");
                    state.muted = !state.muted;

                    midi_main.sendControlChange(VOLUME_CC, state.muted ? MIDI_LOW : MIDI_HIGH, state.midi_channel);
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
}

void handle_midi_in() {
    if(midi_main.read()) {
        DPRINTLN("Read midi_main()");

        digitalWrite(MIDI_MESSAGE_RECIEVED_PIN, HIGH);

        midi_main_countdown = 100;
    }
}

void handle_axe_fx()
{
    // if tuner, handle that
    // get patch num
    // get patch name

    // read axe fx midi
    if (midi_axe_in.read())
    {
        DPRINTLN("Read midi_axe_in()");

        digitalWrite(AXEFX_MESSAGE_RECIEVED_PIN, HIGH);
        midi_axe_countdown = 100;
    }
}

void update_display()
{
    char *line1 = (char *)malloc(sizeof(char) * 16);
    char *line2 = (char *)malloc(sizeof(char) * 16);

    if (state.tuner_active)
    {
        sprintf(line1, "Tuner active");
        sprintf(line2, "%s", clear);
    }
    else if (state.muted) {
        sprintf(line1, "Patch: %d [MUTED]", state.current_patch);
        sprintf(line2, "Scene: %d", state.current_scene);
    }
    else
    {
        sprintf(line1, "Patch: %d", state.current_patch);
        sprintf(line2, "Scene: %d", state.current_scene);
    }

    if (strcmp(last_display1, line1) != 0 || strcmp(last_display2, line2) != 0)
    {
        DPRINT("Updating display: ");
        DPRINT(line1);
        DPRINT("|");
        DPRINTLN(line2);

        clear_display();

        lcd.setCursor(0, 0);
        lcd.print(line1);

        lcd.setCursor(0, 1);
        lcd.print(line2);

        strcpy(last_display1, line1);
        strcpy(last_display2, line2);
    }

    free(line1);
    free(line2);
}

void clear_display()
{
    lcd.setCursor(0, 0);
    lcd.print(clear);
    lcd.setCursor(0, 1);
    lcd.print(clear);
}

void update_leds()
{
    long colour;

    if (state.tuner_active)
    {
        colour = 0x0000ff10;
    }
    else if(state.muted) {
        colour = 0x00ff0000;
    }
    else
    {
        colour = 0x00ff00ff;
    }

    setRGB(1, colour);

    lc.setDigit(0, 0, state.current_patch, midi_axe_countdown > 0);
    lc.setDigit(0, 1, state.current_scene, midi_main_countdown > 0);
}

// colour is bytes in the 0RGB order
void setRGB(int num, long colour) {
    int r = (((1 << 8) - 1) & (colour >> 16));
    int g = (((1 << 8) - 1) & (colour >> 8));
    int b = (((1 << 8) - 1) & (colour));

    analogWrite(RED_PIN, r);
    analogWrite(GREEN_PIN, g);
    analogWrite(BLUE_PIN, b);
}

void handleSystemExclusive(byte* array, unsigned size) {

}

void update_state() {
    if(midi_axe_countdown > 0) {
        midi_axe_countdown--;
    }

    if(midi_main_countdown > 0) {
        midi_main_countdown--;
    }
}