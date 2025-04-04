#include <windows.h>
#include <shellapi.h>
#include <iostream>

// Global variables
NOTIFYICONDATA nid;
HMENU hMenu;

// Function to handle system tray messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 1: // Refresh
            MessageBoxA(NULL, "Refresh clicked!", "System Tray", MB_OK);
            break;
        case 2: // Exit
            Shell_NotifyIcon(NIM_DELETE, &nid); // Remove the tray icon
            PostQuitMessage(0); // Exit the program
            break;
        }
        break;

    case WM_USER + 1: // Custom message for tray icon
        if (lParam == WM_RBUTTONDOWN) // Right-click
        {
            // Show the context menu
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
        }
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void HideIntoSystemTray()
{
    // Hide the console window
    HWND hConsole = GetConsoleWindow();
    ShowWindow(hConsole, SW_HIDE);

    // Create a hidden window to handle messages
    WNDCLASSA wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "TrayIconWindow";
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowA("TrayIconWindow", "Hidden Window", 0, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);

    // Add the tray icon
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_USER + 1; // Custom message for tray icon
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Default application icon
    strcpy_s(nid.szTip, "My Console Program");
    Shell_NotifyIcon(NIM_ADD, &nid);

    // Create the context menu
    hMenu = CreatePopupMenu();
    AppendMenuA(hMenu, MF_STRING, 1, "Refresh");
    AppendMenuA(hMenu, MF_STRING, 2, "Exit");

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}