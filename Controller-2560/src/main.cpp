#include <Arduino.h>
#include <MIDI.h>
#include <Keypad.h>
#include "LiquidCrystal.h"

#include "main.h"
#include "types.h"
#include "io.h"

// move to eeprom
struct patch_t patches[5] = {
    {"patch 1", 0, 1, -1, 0},
    {"patch 2", 1, 2, -1, 0},
    {"patch 3", 2, 3, -1, 0},
    {"patch 4", 3, 4, -1, 0},
    {"patch 5", 4, 5, -1, 0}};

// channel, patch, scene, bank, tuner
struct state_t state = {
    2, 0, 1, 0, false};

struct midi_message_t last_axe_message;
struct midi_message_t last_main_message;

const char *clear = "                ";
char last_display1[32];
char last_display2[32];

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, midi_main);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, midi_axe_in);

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup()
{
    strcpy(last_display1, clear);
    strcpy(last_display2, clear);

    Serial.begin(9600);
    DPRINTLN("Starting");

    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    pinMode(MIDI_MESSAGE_RECIEVED_PIN, OUTPUT);

    lcd.begin(16, 2);
    lcd.setCursor(0, 0);

    lcd.print("Starting...");

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
}

void handle_input() {
    digitalWrite(MIDI_MESSAGE_RECIEVED_PIN, LOW);

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
                }

                if (patch.cc_num > -1)
                {
                    midi_main.sendControlChange(patch.cc_num, patch.cc_data, state.midi_channel);
                }
            }

            // 5 - 9 it's a scene change
            if (value >= 5)
            {
                state.current_scene = value - 4; // scenes are 1 - 5
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
}

void handle_midi_in() {
    if(midi_main.read()) {
        DPRINTLN("Read midi_main()");

        digitalWrite(MIDI_MESSAGE_RECIEVED_PIN, HIGH);
#ifdef DEBUG
        delay(50);
#endif
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

        if (midi_axe_in.getChannel() == state.midi_channel)
        {
            struct midi_message_t message = {
                midi_axe_in.getType(),
                midi_axe_in.getData1(),
                midi_axe_in.getData2()};
        }
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
    int rgb[3];

    if (state.tuner_active)
    {
        rgb[0] = 0x0;
        rgb[1] = 0xff;
        rgb[2] = 0x10;
    }
    else
    {
        rgb[0] = 0x80;
        rgb[1] = 0x00;
        rgb[2] = 0x80;
    }

    analogWrite(RED_PIN, rgb[0]);
    analogWrite(GREEN_PIN, rgb[1]);
    analogWrite(BLUE_PIN, rgb[2]);
}