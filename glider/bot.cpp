#include <stdio.h>

#include "windows.h"
#include "bot.h"
#include "entity_manager.h"
#include "game.h"

bool Bot::running;
bool Bot::hide_menu;
ImGuiTextBuffer Bot::log_buffer;
bool Bot::scroll_to_bottom;
Cheats Bot::cheats;

void Bot::init() {
	cheats.unlock_lua();
	setup_theme();
}

void Bot::draw_menu() {
    if (hide_menu) return;

    ImGui::Begin("Glider");

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("#tabs", tab_bar_flags)) {
		if (ImGui::BeginTabItem("Main")) {
			{
				if (!running) {
					if (ImGui::Button("Start")) test();
				}
				else {
					if (ImGui::Button("Stop")) test();
				}
				ImGui::SameLine();
				if (ImGui::Button("Clear logs")) log_buffer.clear();
			}

			ImGui::Separator();

			{
				ImGui::BeginChild("scrolling");
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
				ImGui::TextUnformatted(log_buffer.begin());
				if (scroll_to_bottom) ImGui::SetScrollHere(1.0f);
				scroll_to_bottom = false;
				ImGui::PopStyleVar();
				ImGui::EndChild();
			}

			ImGui::EndTabItem();
		} 
		if (ImGui::BeginTabItem("Settings")) {
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Cheats")) {
			if (ImGui::Checkbox("Teleport", &cheats.teleport_on)) cheats.teleport();
			if (ImGui::Checkbox("No Fall Damage", &cheats.no_fall_damage_on)) cheats.no_fall_damage();
			if (ImGui::Checkbox("Super Fly", &cheats.super_fly_on)) cheats.super_fly();
			if (ImGui::SliderFloat("Jump Gravity", &cheats.new_jump_grav, -100.0f, 1.0f, "%.2f"))
				write_to_memory((u8*)cheats.jump_gravity_ptr,  (u8*) & cheats.new_jump_grav, sizeof(float));
			if (ImGui::SliderFloat("Wall Climb", &cheats.new_wall_climb, -10.0f, 10.0f, "%.2f"))
				write_to_memory((u8*)cheats.wall_climb_ptr,  (u8*) & cheats.new_wall_climb, sizeof(float));
			if (ImGui::SliderFloat("Player Speed", &cheats.new_speed, 0.0f, 300.0f, "%.2f")) cheats.speed_hack();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Lua")) {
			static char lua_code[2048];
			if (ImGui::Button("Run")) {
				Game::run_lua(lua_code, "a");
			}
			ImGui::SameLine();
			if (ImGui::Button("Load file...")) {
				// TODO
			}
			ImGui::InputTextMultiline("##source", lua_code, 2048, ImVec2(-1, -1));
			ImGui::EndTabItem();
		}
    }
	ImGui::EndTabBar();

    ImGui::End();
}

void Bot::log(const char *fmt, ...) {
    char buf[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    log_buffer.append(buf);
    scroll_to_bottom = true;
}

void Bot::test() {
    Entity_Manager em;
    em.populate_lists();
    em.log();
    Local_Player* lp = &em.local_player;
    auto enemy = lp->select_closest_enemy();
    lp->click_to_move(enemy.get_position());
}
