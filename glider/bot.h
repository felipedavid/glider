#pragma once
#include <stdarg.h>

#include "imgui\imgui.h"
#include "cheats.h"
#include "entity_manager.h"

enum State {
    NONE_STATE,
    GRIND_STATE,
    MOVE_STATE,
    COMBAT_STATE,
};

struct Bot {
    static Entity_Manager em;
    static State current_state;
    static bool running;

    static Cheats cheats;

    // Gui stuff
    static bool hide_menu;
    static ImGuiTextBuffer log_buffer;
    static bool scroll_to_bottom;
    static int tick_rate;
    std::vector<std::string> possible_targets;
    std::vector<std::string> targets;

    static void init();
    static void main_loop();
    static void update();
	static void draw_menu();
    static void log(const char *fmt, ...);
    static void test();
};
