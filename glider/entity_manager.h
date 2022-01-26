#pragma once
#include <vector>

#include "common.h"

enum Entity_Type {
	ET_NONE,
	ET_ITEM,
	ET_CONTAINER,
	ET_UNIT,
	ET_PLAYER,
	ET_GAMEOBJ,
	ET_DYNOBJ,
	ET_CORPSE,
};

struct Entity {
	// Offsets
	static const u32 type_offset = 0x14;
	static const u32 guid_offset = 0x30;

	u32 base_addr;

	Entity(u32 base_addr);
	u64 get_guid();
	Entity_Type get_type();
};

struct Unit : public Entity {
	static const u32 descriptor_ptr_offset = 0x8;
	static const u32 health_offset = 0x58;
	static const u32 name_offset = 0xB30;

	using Entity::Entity;
	u32 get_descriptor_ptr();
	int get_health();
	char* get_name();
};

struct Entity_Manager {
	static std::vector<Unit> unities;

	static int __fastcall process_entity(void* thiss, int filter, u64 guid);
	void populate();
	void log();
};
