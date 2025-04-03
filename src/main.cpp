/* https://www.yamllint.com */
#include <iostream>
#include "loop.h"
#include "tray.h"
#include "fileCheck.h"

/* Check for windows */
bool allow_execution =
#ifdef _WIN32
    true;
#else
    false;
#endif

/* main */
int main()
{
    if (!allow_execution) { return 0; }
    IsFileAvaiable();
    RefreshConfig();
    GetFileLastWriteTime();

    if (system_tray) { HideIntoSystemTray(); }
    if (!enable_closebutton && !enable_timer) { close_button = true; }

    while (!exit_loop)
    {
        if (enable_closebutton) { exit_loop = CloseButtonMain(); }
        if (enable_timer) { exit_loop = TimerMain(); }
    }

    return 0;
}