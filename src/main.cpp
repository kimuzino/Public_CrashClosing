/* https://www.yamllint.com */
#include <iostream>
#include <windows.h>
#include <filesystem>
#include <fstream>
#include <thread>
#include "func.h"

/* Check for windows */
bool allow_execution =
#ifdef _WIN32
    true;
#else
    false;
#endif

void IsFileAvaiable()
{
    if (std::filesystem::exists("config.yaml"))
    {
        return;
    }
    else
    {
        std::fstream file("config.yaml", std::ios::out);
        if (file.is_open())
        {
            file << "Applications:\n";
            file << "  - \"example.exe\"\n";
            file << "EnableTimer: false\n";
            file << "Timer: null\n";
            file << "EnableCloseButton: true\n";
            file << "CloseButton: 0x2E\n";
            file << "SystemTray: false\n";
            file << "CloseOnly: false\n";
            file.close();
        }
        else
        {
            std::cerr << "Failed to create config.yaml\n";
        }
    }

    return;
}

/* main */
int main()
{
    if (!allow_execution) { return 0; }
    close_only = false;

    DWORD processId = 0;
    if (IsProcessRunning("CrashClosing.exe", processId, "multi"))
    {
        std::cerr << "Another instance is already running with PID: " << processId << "\n";
        return 1;
    }

    #define IDI_ICON 101
    #define IDI_SMALL 102
    HINSTANCE hInstance = GetModuleHandle(NULL);
    HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
    HICON hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

    IsFileAvaiable();
    RefreshConfig();
    GetFileLastWriteTime();

    if (close_only) { CloseOnlyMain(); } // If open_only is true, exit the program.

    if (system_tray)
    { 
        tray_thread = std::thread(HideIntoSystemTray);
        tray_thread.detach();
    }
    
    if (!enable_closebutton && !enable_timer) { close_button = true; }

    while (!exit_loop)
    {
        if (enable_closebutton) { exit_loop = CloseButtonMain(); }
        if (enable_timer) { exit_loop = TimerMain(); }
    }

    if (tray_thread.joinable())
    {
        tray_thread.join();
    }

    return 0;
}