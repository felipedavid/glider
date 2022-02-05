#include <Windows.h>
#include <d3d9.h>
#include "kiero/kiero.h"
#include "kiero/minhook/include/MinHook.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"

#include "bot.h"

#define WINDOW_NAME "Glider"

typedef long(__stdcall* EndScene)(LPDIRECT3DDEVICE9);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND window_handle, UINT msg, WPARAM w_param, LPARAM l_param);

EndScene end_scene = NULL;
WNDPROC wnd_proc;
static HWND window = NULL;

void init_imgui(LPDIRECT3DDEVICE9 p_device) {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(p_device);
}

bool init = false;
bool bot_init = false;
Bot bot;

long __stdcall hook_end_scene(LPDIRECT3DDEVICE9 p_device) {
	if (!init) {
		init_imgui(p_device);
		init = true;
	}

	if (!bot_init) {
		bot.init();
		bot_init = true;
	}

	// Toggle menu when user press the END key
	if (GetAsyncKeyState(VK_END) & 1) bot.hide_menu = !bot.hide_menu;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	bot.draw_menu();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	return end_scene(p_device);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	// If the bot's menu is not hidden, pass all they input to the ImGui window buffer
	if (!bot.hide_menu) {
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		return true;
	}

	return CallWindowProc(wnd_proc, hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam) {
	DWORD wnd_proc_id;
	GetWindowThreadProcessId(handle, &wnd_proc_id);

	if (GetCurrentProcessId() != wnd_proc_id) return TRUE;

	window = handle;
	return FALSE; // window found abort search
}

HWND get_proc_window() {
	window = NULL;
	EnumWindows(enum_windows_callback, NULL);
	return window;
}

DWORD WINAPI main_thread(LPVOID lpReserved)
{
	bool attached = false;
	do {
		if (kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success) {
			kiero::bind(42, (void**)& end_scene, hook_end_scene);

			do window = get_proc_window();
			while (window == NULL);

			wnd_proc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)WndProc);
			attached = true;
		}
	} while (!attached);
	return TRUE;
}

BOOL WINAPI DllMain(HMODULE module_handle, DWORD reason, LPVOID reserved)
{
	switch (reason) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(module_handle);
		CreateThread(nullptr, 0, main_thread, module_handle, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}
