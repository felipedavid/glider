#include "common.h"
#include "game.h"
#include "entity_manager.h"
#include "bot.h"

std::unordered_map<u64, Unit> Entity_Manager::unit_list;
std::unordered_map<u64, Player> Entity_Manager::player_list;
Local_Player Entity_Manager::local_player = 0;

Entity::Entity(u32 base_addr) {
	this->base_addr = base_addr;
}

u64 Entity::get_guid() {
	return read<u64>(base_addr + guid_offset);
}

Entity_Type Entity::get_type() {
	return read<Entity_Type>(base_addr + type_offset);
}

u32 Unit::get_descriptor_ptr() {
	return read<u32>(base_addr + descriptor_ptr_offset);
}

int Unit::get_health() {
	return read<int>(get_descriptor_ptr() + health_offset);
}

const char* Unit::get_name() {
	return (char*)read<u32>(read<u32>(base_addr + name_offset));
}

Vec3 Unit::get_position() {
	return read<Vec3>(base_addr + position_offset);
}

Creature_Reaction Unit::get_reaction(u32 player_ptr) {
	return (Creature_Reaction) Game::get_unit_reaction(base_addr, base_addr, player_ptr);
}

int Unit::get_mana() {
    return read<u32> (get_descriptor_ptr() + mana_offset);
}

int Unit::get_level() {
    return read<u32> (get_descriptor_ptr() + level_offset);
}

bool Unit::is_casting() {
    return (bool) read<u32>(base_addr + current_spellcast_offset);
}

bool Unit::can_be_looted() {
	return get_health() == 0 && read<u32>(get_descriptor_ptr() + dynamic_flags_offset) == CAN_BE_LOOTED;
}

std::vector<u32> Unit::get_buff_ids() {
    std::vector<u32> buff_ids(10, 0);

    u32 current_buff_offset = buffs_base_offset;
    for (int i = 0; i < 10; i++) {
        u32 buff_id = read<u32>(get_descriptor_ptr() + current_buff_offset);
        if (buff_id == 0) break; 

        buff_ids[i] = buff_id;
        current_buff_offset += 4;
    }
    return buff_ids;
}

std::vector<u32> Unit::get_debuff_ids() {
    std::vector<u32> debuff_ids(16, 0);

    u32 current_debuff_offset = debuffs_base_offset;
    for (int i = 0; i < 16; i++) {
        u32 debuff_id = read<u32>(get_descriptor_ptr() + current_debuff_offset);
        if (debuff_id == 0) break;

        debuff_ids[i] = debuff_id;
        current_debuff_offset += 4;
    }
    return debuff_ids;
}

bool Unit::has_buff(const char *buff_name) {
    auto ids = get_buff_ids();
    for (auto id : ids) {
        if (id && !strcmp(buff_name, get_spell_name(id))) {
            return true;
        }
    }
    return false;
}

const char* Player::get_name() {
	u32 name_ptr = read<u32>(name_base_offset);
	for (;;) {
		u64 next_guid = read<u64>(name_ptr + next_name_offset);
		if (next_guid != get_guid()) {
			name_ptr = read<u32>(name_ptr);
		} else break;
	}
	return (const char*) (name_ptr + player_name_offset);
}

u64 Local_Player::get_guid() {
	return Game::get_player_guid();
}

float Local_Player::distance_to(Vec3 pos) {
	Vec3 me_position = get_position();
	float delta_x = me_position.x - pos.x;
	float delta_y = me_position.y - pos.y;
	float delta_z = me_position.z - pos.z;

	return (float)sqrt(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);
}

Unit Local_Player::select_closest_enemy() {
	Unit enemy = Entity_Manager::unit_list.begin()->second;
	for (auto unit : Entity_Manager::unit_list) {
		auto type = unit.second.get_type();
		auto react = unit.second.get_reaction(base_addr);
		if (type != UT_CRITTER && type != UT_NOT_SPECIFIED && type != UT_TOTEM &&
			(react == CR_HOSTILE || react == CR_UNFRIENDLY || react == CR_NEUTRAL) &&
			(unit.second.get_health() > 0) && (distance_to(enemy.get_position()) >
				distance_to(unit.second.get_position()))) {
			enemy = unit.second;
		}
	}
	set_target(enemy.get_guid());
	return enemy;
}

void Local_Player::set_target(u64 guid) { Game::set_target(guid); }

void Local_Player::click_to_move(Vec3 pos) {
	u64 interact_guid_ptr = 0;
	Game::click_to_move(base_addr, base_addr, CT_MOVE, &interact_guid_ptr, &pos, 2);
}

void Local_Player::click_to_stop() {
    u64 interact_guid_ptr = 0;
    Vec3 pos = get_position();
    Game::click_to_move(base_addr, base_addr, CT_NONE, &interact_guid_ptr, &pos, 2);
}

void Local_Player::refresh_spells() {
    spells.erase(spells.begin(), spells.end());
    for (u32 *s_id = (u32*) player_spells_base_addr, i = 0; *s_id != 0 && i < 1024; s_id++) {
        const char *name = get_spell_name(*s_id);
        spells[std::string(name)] = *s_id;
    }
}

void Local_Player::face_entity(u64 guid) {
    auto p = get_position();
    Game::click_to_move(base_addr, base_addr, CT_FACE_TARGET, &guid, &p, 2);
}

void Local_Player::try_use_ability(const char *name, int mana_required) {
    if (is_spell_ready(name, 0) && !is_casting() && get_mana() >= mana_required) {
        cast_spell(name);
    }
}

void Local_Player::cast_spell(const char *name) {
    char lua_code[256];
    snprintf(lua_code, sizeof(lua_code), "CastSpellByName('%s')", name);
    Game::run_lua(lua_code, "Nothing");
}

bool Local_Player::is_spell_ready(const char* name, int spell_rank) {
    if (spells.find(std::string(name)) == spells.end()) return false;
    return Game::is_spell_ready(spells[name]);
}

int Entity_Manager::process_entity(void* thiss, int filter, u64 guid) {
	u32 base_addr = Game::get_entity_ptr(guid);
	Entity_Type type = read<Entity_Type>(base_addr + 0x14);

	switch (type) {
	case ET_UNIT: {
		unit_list.insert({ guid, base_addr });
    } break;
	case ET_PLAYER:
		if (guid == local_player.get_guid()) {
			local_player.base_addr = base_addr;
		} else {
			player_list.insert({ guid, base_addr });
		}
		break;
	}

	return 1;
}

void Entity_Manager::populate_lists() {
	unit_list.clear();
	player_list.clear();
	Game::enumerate_visible_entities((void*)process_entity, 0);
}

void Entity_Manager::log() {
    for (auto unit : unit_list) {
		Bot::log("Nome: %s\n", unit.second.get_name());
		Bot::log("Guid: %llu\n", unit.first);
		Bot::log("Vida: %d\n", unit.second.get_health());
		Bot::log("Ponteiro: 0x%x\n", unit.second.base_addr);
	}

    for (auto player : player_list) {
		Bot::log("Nome: %s\n", player.second.get_name());
		Bot::log("Guid: %llu\n", player.first);
		Bot::log("Vida: %d\n", player.second.get_health());
		Bot::log("Ponteiro: 0x%x\n\n", player.second.base_addr);
	}
}
