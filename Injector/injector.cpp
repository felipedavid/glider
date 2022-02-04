#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#define DLL_PATH "C:\\work\\glider\\Debug\\Glider.dll"

void setup_debug_privileges() {
    printf("[*] Setting up debug privileges...\n");

    LUID luid;
    TOKEN_PRIVILEGES tp;
    HANDLE token_handle;
    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token_handle);
    LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges(token_handle,
        FALSE,
        &tp,
        0,
        (PTOKEN_PRIVILEGES)NULL,
        (PDWORD)NULL);
}

HANDLE get_proc_handle_by_window_name(const char* window_name) {
    printf("[*] Looking for WoW's Window.\n");
    HWND window_handle = FindWindowA(NULL, window_name);
    if (!window_handle) {
        printf("[!] Window not found.\n");
        system("PAUSE");
        exit(1);
    }

    DWORD proc_id;
    GetWindowThreadProcessId(window_handle, &proc_id);

    return OpenProcess(PROCESS_CREATE_THREAD |
        PROCESS_QUERY_INFORMATION |
        PROCESS_VM_OPERATION |
        PROCESS_VM_WRITE |
        PROCESS_VM_READ,
        FALSE,
        proc_id);
}

BOOL inject_dll() {
    HANDLE proc_handle = get_proc_handle_by_window_name("World of Warcraft");

    printf("[*] Injecting...\n");
    // Allocate space for dll's path in the process address space
    size_t dll_path_len = strlen(DLL_PATH) + 1;
    void* allocated_space = VirtualAllocEx(proc_handle,
        NULL,
        dll_path_len,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE);
    if (!allocated_space) {
        printf("[!] Could not allocate space on the process's memory.\n");
        system("PAUSE");
        exit(1);
    }

    // Write the dll path into the allocated space
    WriteProcessMemory(proc_handle, allocated_space, DLL_PATH, dll_path_len, NULL);

    // Find the LoadLibrary function and create a thread to run it with our dll path
    void* load_lib_ptr = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
    HANDLE th = CreateRemoteThread(proc_handle,
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)load_lib_ptr,
        allocated_space,
        0,
        NULL);
    if (!th) {
        printf("[!] Could not create remote thread.\n");
        system("PAUSE");
        exit(1);
    }

    VirtualFreeEx(proc_handle, allocated_space, dll_path_len, MEM_DECOMMIT | MEM_RELEASE);
    CloseHandle(th);

    return TRUE;
}

int main() {
    setup_debug_privileges();
    inject_dll();
    system("PAUSE");
}