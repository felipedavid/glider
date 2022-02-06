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

enum Dynamic_Flag {
	UNTOUCHED,
	CAN_BE_LOOTED,
	IS_MARKED,
	TAPPED,
	TAPPED_BY_ME,
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
    static const u32 health_offset         = 0x58;
    static const u32 max_health_offset     = 0x70;
    static const u32 name_offset           = 0xB30; 
    static const u32 position_offset       = 0x9B8;
    static const u32 facing_offset         = 0x9C4;
    static const u32 level_offset          = 0x88;
    static const u32 rage_offset           = 0x60;
    static const u32 mana_offset           = 0x5C;
    static const u32 dynamic_flags_offset  = 0x23C;
    static const u32 buffs_base_offset     = 0xBC; 
    static const u32 debuffs_base_offset   = 0x13C; 
    static const u32 current_spellcast_offset = 0xC8C;

	using Entity::Entity;
	u32 get_descriptor_ptr();
	int get_health();
	const char* get_name();
	Vec3 get_position();
	Creature_Reaction get_reaction(u32 player_ptr);
    int get_mana();
    int get_level();
	bool is_casting();
	bool can_be_looted();
    bool has_buff(const char *buff_name);
    std::vector<u32> get_buff_ids();
    std::vector<u32> get_debuff_ids();
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
	static const u32 player_spells_base_addr = 0xB700F0;

    std::unordered_map<std::string, u32> spells;

	using Player::Player;
	u64 get_guid();
	float distance_to(Vec3 pos);
	Unit select_closest_enemy();
	void set_target(u64 guid);
	void click_to_move(Vec3 pos);
    void click_to_stop();
    void refresh_spells();
    void face_entity(u64 guid);
    void try_use_ability(const char *name, int mana_required);
	void cast_spell(const char* name);
    bool is_spell_ready(const char *name, int spell_rank);
};

struct Entity_Manager {
    static std::unordered_map<u64, Unit> unit_list;
    static std::unordered_map<u64, Player> player_list;
	static Local_Player local_player;

	static int __fastcall process_entity(void* thiss, int filter, u64 guid);
	void populate_lists();
	void log();
};
