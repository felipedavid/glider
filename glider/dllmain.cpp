#include "pch.h"
#include <stdio.h>

#include "game.h"
#include "entity_manager.h"

void start_point(HMODULE module_handle) {
    FILE* f;
    AllocConsole();
    freopen_s(&f, "CONOUT$", "w", stdout);

    printf("Welcome to Glider!");

    Entity_Manager em;

    while (true) {
        if (GetAsyncKeyState(VK_F1) & 1) {
            em.populate();
            em.log();
        }

        if (GetAsyncKeyState(VK_END) & 1) break;
        Sleep(100);
    }

    // Fechar console e descarregar DLL
	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(module_handle, 0);
}

BOOL APIENTRY DllMain( HMODULE module_handle,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)start_point, module_handle, 0, NULL);
        break;
    }
    return TRUE;
}

