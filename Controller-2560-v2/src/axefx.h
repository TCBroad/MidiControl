//
// Created by tombr on 09/11/2018.
//

#ifndef CONTROLLER_2560_V2_AXEFX_H
#define CONTROLLER_2560_V2_AXEFX_H

#include <Arduino.h>

#define SCENE_CHANGE_CC 34
#define TUNER_CC 15
#define VOLUME_CC 21

#define AXE_HEADER_BYTES 0x00, 0x01, 0x74, 0x03

const byte get_firmware_version[6] = { AXE_HEADER_BYTES, 0x08, 0x0e };
const byte get_patch_name[6] = { AXE_HEADER_BYTES, 0x0F, 0x09 };
const byte get_patch_num[6] = { AXE_HEADER_BYTES, 0x14, 0x12 };
const byte get_scene_num[7] = { AXE_HEADER_BYTES, 0x29, 0x7F, 0x50 };

void get_preset_name(char* buffer, byte* data, unsigned length);

#endif //CONTROLLER_2560_V2_AXEFX_H
