#include "common.h"
#include "game.h"

struct Cheats {
	bool unlock_lua_on = false;
	bool teleport_on = false;
	bool no_fall_damage_on = false;
	bool super_fly_on = false;

	float *jump_gravity_ptr = (float *)0x007C6272;
	float new_jump_grav = *jump_gravity_ptr;
	float *wall_climb_ptr = (float*)0x0080DFFC;
	float new_wall_climb = *wall_climb_ptr;
	u8* fall_damage_ptr = (u8*)0x007C63DA;
	u8* super_fly_ptr = (u8*)0x006341BC;
	float new_speed;

	void unlock_lua();
	void teleport();
	void no_fall_damage();
	void super_fly();
	void speed_hack();
};