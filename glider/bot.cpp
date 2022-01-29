#include <stdio.h>

#include "bot.h"
#include "entity_manager.h"

bool Bot::running;
bool Bot::hide_menu;
ImGuiTextBuffer Bot::log_buffer;
bool Bot::scroll_to_bottom;

void Bot::init() {
}

void Bot::draw_menu() {
    if (hide_menu) return;

    ImGui::Begin("Lazy Bot");
    {
        if (ImGui::Button("Test")) test();
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
    em.populate();
    em.log();
}
