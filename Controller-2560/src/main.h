#ifndef MAIN_H
#define MAIN_H

#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.

#ifdef DEBUG    //Macros are usually in all capital letters.
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
#else
  #define DPRINT(...)     //now defines a blank line
  #define DPRINTLN(...)   //now defines a blank line
#endif

void handle_input();
void handle_midi_in();
void update_display();
void clear_display();
void update_leds();
void handle_axe_fx();
void setRGB(int num, long colour);

void handleSystemExclusive(byte* array, unsigned size);

#endif