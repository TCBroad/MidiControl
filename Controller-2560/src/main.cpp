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
    {"patch 1", 0, 0, -1, 0},
    {"patch 2", 1, 1, -1, 0},
    {"patch 3", 2, 2, -1, 0},
    {"patch 4", 3, 3, -1, 0},
    {"patch 5", 4, 4, -1, 0}
};

// channel, patch, scene, bank, tuner
struct state_t state = {
    2, 0, 1, 0, false, false, (char*)"                "
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
unsigned long lcd_countdown = 0;
unsigned long led_countdown = 0;

bool initialised = false;

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

    midi_main.begin(state.midi_channel);
    midi_axe_in.begin(state.midi_channel);
    midi_axe_in.setHandleSystemExclusive(handleSystemExclusive);
    midi_main.setHandleSystemExclusive(handleSystemExclusive);

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
    lc.setChar(0, 0, '_', false);
    lc.setChar(0, 1, '_', false);

    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, HIGH);

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Checking axe fx");
    midi_main.sendSysEx(6, get_firmware_version);

    led_countdown = 1000;
    lcd_countdown = 10000;
}

void loop()
{
    handle_midi_in();
    handle_axe_fx();
    handle_input();
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
                    midi_main.sendControlChange(SCENE_CHANGE_CC, 0, state.midi_channel);

                    state.current_scene = 0;
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
                    state.current_scene = value; // scenes are 5 - 9                    
                } else {
                    state.current_scene = value - 5; // scenes are 0 - 4
                }

                state.tuner_active = false;

                midi_main.sendControlChange(SCENE_CHANGE_CC, state.current_scene, state.midi_channel);
            }
        }
        else
        {
            // command button
            byte sysex[] = { 0x00, 0x01, 0x74, 0x03, 0x08, 0x0e };
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

                    midi_main.sendSysEx(6, sysex, false);
                    for (int i = 0; i < 6; i++) {
                        DPRINT(sysex[i]);
                        DPRINT(", ");
                    }

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
        // DPRINTLN("Read midi_main()");

        digitalWrite(MIDI_MESSAGE_RECIEVED_PIN, HIGH);

        midi_main_countdown = 100;
    }
}

void handle_axe_fx()
{
    if (midi_axe_in.read())
    {
        // DPRINTLN("Read midi_axe_in()");

        digitalWrite(AXEFX_MESSAGE_RECIEVED_PIN, HIGH);
        midi_axe_countdown = 100;
    }
}

void update_display()
{
    if(lcd_countdown > 0) {
        return;
    }

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

        writeText(line1, line2);

        strcpy(last_display1, line1);
        strcpy(last_display2, line2);
    }

    free(line1);
    free(line2);
}

void writeText(char* line1, char* line2) {
    clear_display();

    lcd.setCursor(0, 0);
    lcd.print(line1);

    lcd.setCursor(0, 1);
    lcd.print(line2);
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

    if(led_countdown == 0) {        
        lc.setDigit(0, 0, state.current_patch, midi_axe_countdown > 0);
        lc.setDigit(0, 1, state.current_scene, midi_main_countdown > 0);
    }
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

void handleSystemExclusive(byte* data, unsigned size) {
    // DPRINTLN("Sysex handle()");

    if(size < 6) {
        DPRINTLN("Invalid message!");

        return;
    }

    char* buffer = (char*)malloc(sizeof(char) * 16);
    // const byte* data = midi_axe_in.getSysExArray();
    // unsigned char data_length = midi_axe_in.getSysExArrayLength();


    switch(data[5]) {
        case AXE_ID_FIRMWARE:
            DPRINTLN("Firmware response");

            sprintf(buffer, "Firmware: %d.%d", data[6], data[7]);
            writeText((char*)"Axe FX connected", buffer);
            lcd_countdown = 5000;

            // connected to axe, get some info
            midi_main.sendSysEx(6, get_patch_num);
            delay(50);

            break;
        case AXE_ID_PRESETNAME:
            DPRINTLN("Preset name response");
            get_preset_name(state.patch_name, data, size);

            initialised = true;
            break;
        case AXE_ID_PRESETNUM:
            DPRINTLN("Preset number response");
            if (!initialised) {
                state.current_patch = data[6];
                midi_main.sendSysEx(7, get_scene_num);
                delay(50);
            }

            break;
        case AXE_ID_SCENE:
            DPRINTLN("Scene number response");
            if (!initialised) {
                state.current_scene = data[6] + 1; // 0 - 7
                midi_main.sendSysEx(6, get_patch_name);
                delay(50);
            }

            break;
        case AXE_ID_TEMPO:
            // DPRINTLN("Tempo response");
            
            break;
    }
}

void update_state() {
    if(midi_axe_countdown > 0) {
        midi_axe_countdown--;
    }

    if(midi_main_countdown > 0) {
        midi_main_countdown--;
    }

    if(lcd_countdown > 0) {
        lcd_countdown--;
    }
    
    if(led_countdown > 0) {
        led_countdown--;
    }
}