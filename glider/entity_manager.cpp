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

int Entity_Manager::process_entity(void* thiss, int filter, u64 guid) {
	u32 base_addr = Game::get_entity_ptr(guid);
	Entity_Type type = read<Entity_Type>(base_addr + 0x14);

	switch (type) {
	case ET_UNIT:
		unit_list.insert({ guid, base_addr });
		break;
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
		Bot::log("Ponteiro: 0x%x\n\n", unit.second.base_addr);
	}

    for (auto player : player_list) {
		Bot::log("Nome: %s\n", player.second.get_name());
		Bot::log("Guid: %llu\n", player.first);
		Bot::log("Vida: %d\n", player.second.get_health());
		Bot::log("Ponteiro: 0x%x\n\n", player.second.base_addr);
	}
}
