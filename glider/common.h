#pragma once
#include <stdint.h>

typedef int64_t u64;
typedef int32_t u32;
typedef int16_t u16;

struct Vec3 { float x, y, z; };

template <typename T>
T read(u32 addr) {
	return *(T*)addr;
}