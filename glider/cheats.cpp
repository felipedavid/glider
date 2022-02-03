#include "cheats.h"

void Cheats::unlock_lua() {
	if (unlock_lua_on) return;
	u8 patch[] = { 0xB8, 0x01, 0x00, 0x00, 0x00, 0xc3 };
	write_to_memory((u8*)0x494A50, (u8*)patch, 6);
}

void Cheats::teleport() {
	if (teleport_on) return;
}

void Cheats::no_fall_damage() {
	if (no_fall_damage_on) {
		static u8 on[] = { 0x31, 0xC9, 0x90 };
		write_to_memory(fall_damage_ptr, on, sizeof(on));
	}
	else {
		static u8 off[] = { 0x8B, 0x4F, 0x78 };
		write_to_memory(fall_damage_ptr, off, sizeof(off));
	}
}

void Cheats::super_fly() {
	if (super_fly_on) {
		static u8 on[] = { 0x90, 0x90 };
		write_to_memory(super_fly_ptr, on, sizeof(on));
		static u8 swim[] = { 0x00, 0x20 };
		write_to_memory((u8*)0x007C620D, swim, sizeof(swim));
	} else {
		static u8 off[] = { 0x74, 0x25 };
		write_to_memory(super_fly_ptr, off, sizeof(off));
		static u8 no_swim[] = { 0x20, 0x00 };
		write_to_memory((u8*)0x007C620D, no_swim, sizeof(no_swim));
	}
}

void Cheats::speed_hack() {
	u32 player_base = Game::get_entity_ptr(Game::get_player_guid());
	static u32 movement_speed_offset = 0xA34;
	write_to_memory((u8*)(player_base + movement_speed_offset), (u8*)&new_speed, sizeof(float));
}