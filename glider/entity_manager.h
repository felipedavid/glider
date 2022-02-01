#pragma once
#include <vector>
#include <unordered_map>

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

enum Creature_Type {
	UT_BEAST = 1,
	UT_DRAGONKIN,
	UT_DEMON,
	UT_ELEMENTAL,
	UT_GIANT,
	UT_UNDEAD,
	UT_HUMANOID,
	UT_CRITTER,
	UT_MECHANICAL,
	UT_NOT_SPECIFIED,
	UT_TOTEM,
};


enum Creature_Reaction {
	CR_HATED,
	CR_HOSTILE,
	CR_UNFRIENDLY,
	CR_NEUTRAL,
	CR_FRIENDLY,
	CR_HONORED,
	CR_REVERED,
	CR_EXALED,
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
	static const u32 position_offset = 0x9B8;

	using Entity::Entity;
	u32 get_descriptor_ptr();
	int get_health();
	const char* get_name();
	Vec3 get_position();
	Creature_Reaction get_reaction(u32 player_ptr);
};

struct Player : public Unit {
	static const u32 name_base_offset = 0xC0E230;
	static const u32 next_name_offset = 0xC;
	static const u32 player_name_offset = 0x14;

	using Unit::Unit;
	const char* get_name();
};

struct Local_Player : public Player {
	static const u32 get_player_guid_fun_ptr = 0x468550;
	static const u32 player_spells_base_addr = 0xB700F0;;

	using Player::Player;
	u64 get_guid();
	float distance_to(Vec3 pos);
	Unit select_closest_enemy();
	void set_target(u64 guid);
	void click_to_move(Vec3 pos);
};

struct Entity_Manager {
    static std::unordered_map<u64, Unit> unit_list;
    static std::unordered_map<u64, Player> player_list;
	static Local_Player local_player;

	static int __fastcall process_entity(void* thiss, int filter, u64 guid);
	void populate_lists();
	void log();
};
