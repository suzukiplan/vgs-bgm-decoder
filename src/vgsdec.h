/* (C)2016, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGSDEC - header
 *    Platform: Common
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#ifndef INCLUDE_VGSDEC_H
#define INCLUDE_VGSDEC_H

#define VGSDEC_REG_KEY_0 0x10
#define VGSDEC_REG_KEY_1 0x11
#define VGSDEC_REG_KEY_2 0x12
#define VGSDEC_REG_KEY_3 0x13
#define VGSDEC_REG_KEY_4 0x14
#define VGSDEC_REG_KEY_5 0x15
#define VGSDEC_REG_TONE_0 0x20
#define VGSDEC_REG_TONE_1 0x21
#define VGSDEC_REG_TONE_2 0x22
#define VGSDEC_REG_TONE_3 0x23
#define VGSDEC_REG_TONE_4 0x24
#define VGSDEC_REG_TONE_5 0x25
#define VGSDEC_REG_VOL_0 0x30
#define VGSDEC_REG_VOL_1 0x31
#define VGSDEC_REG_VOL_2 0x32
#define VGSDEC_REG_VOL_3 0x33
#define VGSDEC_REG_VOL_4 0x34
#define VGSDEC_REG_VOL_5 0x35
#define VGSDEC_REG_PLAYING 0x40
#define VGSDEC_REG_INDEX 0x50
#define VGSDEC_REG_LOOP_INDEX 0x51
#define VGSDEC_REG_LENGTH 0x52
#define VGSDEC_REG_TIME 0x60
#define VGSDEC_REG_LOOP_TIME 0x61
#define VGSDEC_REG_TIME_LENGTH 0x62
#define VGSDEC_REG_LOOP_COUNT 0x70
#define VGSDEC_REG_RESET 0x80
#define VGSDEC_REG_FADEOUT 0x90

#ifndef _WIN32
#define __stdcall
#endif

#ifdef __cplusplus
extern "C" {
#endif

void* __stdcall vgsdec_create_context();
int __stdcall vgsdec_load_bgm_from_file(void* context, const char* path);
int __stdcall vgsdec_load_bgm_from_memory(void* context, void* data, size_t size);
void __stdcall vgsdec_execute(void* context, void* buffer, size_t size);
int __stdcall vgsdec_get_value(void* context, int type);
void __stdcall vgsdec_set_value(void* context, int type, int value);
void __stdcall vgsdec_release_context(void* context);

#ifdef __cplusplus
};
#endif

#endif
