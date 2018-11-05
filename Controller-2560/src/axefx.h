#ifndef AXEFX_H
#define AXEFX_H

#define SCENE_CHANGE_CC 33
#define TUNER_CC 15
#define VOLUME_CC 21

#define AXE_HEADER_BYTES 0x00, 0x01, 0x74, 0x03

#define AXE_ID_FIRMWARE 0x08
#define AXE_ID_PRESETNAME 0x0F
#define AXE_ID_SCENE 0x29
#define AXE_ID_PRESETNUM 0x14
#define AXE_ID_TEMPO 0x10

const byte get_firmware_version[6] = { AXE_HEADER_BYTES, 0x08, 0x0e };
const byte get_patch_name[6] = { AXE_HEADER_BYTES, 0x0F, 0x09 };
const byte get_patch_num[6] = { AXE_HEADER_BYTES, 0x14, 0x12 };
const byte get_scene_num[6] = { AXE_HEADER_BYTES, 0x29, 0x2F };

void get_preset_name(char* buffer, byte* data, unsigned length);

#endif