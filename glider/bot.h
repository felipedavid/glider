#pragma once
#include <stdarg.h>

#include "imgui\imgui.h"
#include "cheats.h"

struct Bot {
    static bool running;

    static Cheats cheats;

    // Gui stuff
    static bool hide_menu;
    static ImGuiTextBuffer log_buffer;
    static bool scroll_to_bottom;

    static void init();
	static void draw_menu();
    static void log(const char *fmt, ...);
    static void test();
};
