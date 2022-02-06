#include <stdio.h>

#include "windows.h"
#include "bot.h"
#include "entity_manager.h"
#include "game.h"
#include "common.h"

Entity_Manager Bot::em;
State Bot::current_state;
bool Bot::running;
bool Bot::hide_menu;
ImGuiTextBuffer Bot::log_buffer;
bool Bot::scroll_to_bottom;
Cheats Bot::cheats;
int Bot::tick_rate;

void Bot::init() {
	cheats.unlock_lua();
	setup_theme();
}

void Bot::main_loop() {
    current_state = GRIND_STATE;
    while (running) {
        run_procedure_on_main_thread((void*)Bot::update);
        Sleep(tick_rate);
    }
}

void Bot::update() {
    static Vec3 prev_pos;
    em.populate_lists();
    Local_Player *lp = &em.local_player;
    lp->refresh_spells();

    static Unit enemy = lp->select_closest_enemy();
    static u64 prev_enemy = enemy.get_guid();
    if (enemy.get_guid() != prev_enemy) {
        prev_enemy = enemy.get_guid();
        prev_pos = lp->get_position();
    }
    switch (current_state) {
        case GRIND_STATE: {
            log("Looking for enemy...");
            enemy = lp->select_closest_enemy();
            lp->face_entity(enemy.get_guid());
            if (enemy.base_addr != 0) {
                current_state = MOVE_STATE;
            }
        } break;
        case MOVE_STATE: {
            if (lp->distance_to(enemy.get_position()) > 5.0) {
                log("Moving to enemy...");
                lp->click_to_move(enemy.get_position());
            } else {
                if (lp->get_position().x == prev_pos.x && lp->get_position().y == prev_pos.y) 
                    lp->click_to_move(enemy.get_position());
                else {
                    lp->click_to_stop();
                    current_state = COMBAT_STATE;
                }
            }
        } break;
        case COMBAT_STATE: {
            lp->try_use_ability("Frost Armor", 60);
            lp->try_use_ability("Fireball", 30);
            lp->try_use_ability("Attack", 0);

            if (enemy.get_health() == 0) {
                if (enemy.can_be_looted()) {
                    if (lp->distance_to(enemy.get_position()) > 5) {
                        lp->click_to_move(enemy.get_position());
                    } else {
                        log("Looting...");
                        Game::right_click_unit(enemy.base_addr, enemy.base_addr, 1);
                    }
                } else {
                    current_state = GRIND_STATE;
                }
            }
            log("In combat...");
            if (enemy.get_health() > 0 && lp->distance_to(enemy.get_position()) < 5) {
                current_state = MOVE_STATE;
            }
        } break;
    }
}

void Bot::draw_menu() {
    if (hide_menu) return;

    ImGui::Begin("Glider");

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("#tabs", tab_bar_flags)) {
		if (ImGui::BeginTabItem("Main")) {
            if (!running) {
                if (ImGui::Button("Start")) {
                    running = true;
                    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Bot::main_loop, NULL, 0, NULL);
                }
            } else {
                if (ImGui::Button("Stop")) running = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear logs")) log_buffer.clear();

            ImGui::BeginChild("scrolling");
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
            ImGui::TextUnformatted(log_buffer.begin());
            if (scroll_to_bottom) ImGui::SetScrollHere(1.0f);
            scroll_to_bottom = false;
            ImGui::PopStyleVar();
            ImGui::EndChild();

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
