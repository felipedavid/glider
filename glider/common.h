#pragma once
#include <stdint.h>

typedef int64_t u64;
typedef int32_t u32;
typedef int16_t u16;
typedef int8_t u8;

struct Vec3 { float x, y, z; };

template <typename T>
T read(u32 addr) {
	return *(T*)addr;
}

void setup_theme();
void write_to_memory(u8* dst, u8* src, int size);
const char* get_spell_name(u32 spell_id);
void run_procedure_on_main_thread(void* procedure);