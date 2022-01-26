#include "pch.h" // windows crap
#include "common.h"
#include "game.h"
#include "entity_manager.h"

std::vector<Unit> Entity_Manager::unities;

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

char* Unit::get_name() {
	return (char*)read<u32>(read<u32>(base_addr + name_offset));
}


int Entity_Manager::process_entity(void* thiss, int filter, u64 guid) {
	u32 base_addr = Game::get_entity_ptr(guid);
	Entity_Type type = read<Entity_Type>(base_addr + 0x14);

	switch (type) {
	case ET_UNIT:
		unities.push_back(Unit{ base_addr });
		break;
	}

	return 1;
}

void Entity_Manager::populate() {
	unities.clear();
	Game::enumerate_visible_entities((void*)process_entity, 0);
}

void Entity_Manager::log() {
	for (int i = 0; i < unities.size(); i++) {
		printf("Entidade %d\n", i+1);
		printf("Nome: %s\n", unities[i].get_name());
		printf("Guid: %llu\n", unities[i].get_guid());
		printf("Vida: %d\n", unities[i].get_health());
		printf("Ponteiro: 0x%x\n\n", unities[i].base_addr);
	}
}