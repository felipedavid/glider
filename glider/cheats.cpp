#include "common.h"
#include "cheats.h"

void Cheats::unlock_lua() {
	if (stt_unlock_lua) return;
	u8 patch[] = { 0xB8, 0x01, 0x00, 0x00, 0x00, 0xc3 };
	write_to_memory((u8*)0x494A50, (u8*)patch, 6);
}