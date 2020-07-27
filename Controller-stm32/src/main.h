//
// Created by tombr on 09/11/2018.
//

#ifndef CONTROLLER_2560_V2_MAIN_H
#define CONTROLLER_2560_V2_MAIN_H

#include <Keypad.h>

#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.

#ifdef DEBUG    //Macros are usually in all capital letters.
#define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
#define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
#define DPRINTF(...)    Serial.print(F(__VA_ARGS__))     //DPRINT is a macro, debug print
#define DPRINTLNF(...)  Serial.println(F(__VA_ARGS__))   //DPRINTLN is a macro, debug print with new line
#else
#define DPRINT(...)     //now defines a blank line
#define DPRINTLN(...)   //now defines a blank line
#define DPRINTF(...)   //now defines a blank line
#define DPRINTLNF(...)   //now defines a blank line
#endif

void handleKeypadEvent(KeypadEvent key);
void handleSystemExclusive(byte *array, unsigned size);

#endif //CONTROLLER_2560_V2_MAIN_H
