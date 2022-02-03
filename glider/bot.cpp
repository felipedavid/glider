#include <stdio.h>

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
