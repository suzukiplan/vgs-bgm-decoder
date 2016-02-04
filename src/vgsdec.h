/* (C)2016, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGSDEC - header
 *    Platform: Common
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#ifndef INCLUDE_VGSDEC_H
#define INCLUDE_VGSDEC_H

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

