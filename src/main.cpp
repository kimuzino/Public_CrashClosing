/* https://www.yamllint.com */
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <array>
#include <string>
#include <windows.h>
#include "loop.h"
#include "tray.h"

/* Check for windows */
bool allow_execution =
#ifdef _WIN32
    true;
#else
    false;
#endif

bool enable_timer; // Defines if the timer will be enabled.
int timer; // Defines in seconds how long the closing will wait. (Default 30 seconds)
bool enable_closebutton; // Defines if the close button will be enabled. (Set to true as default if both enable_timer/button are false)
int close_button; // Defines the button that will close app`s. (Default Delete (0x2E) key)
bool system_tray; // Defines if the software will be hidden into system tray or shown in cmd format.

const int D_close_button = 0x2E; // Defines the default key (Delete) for close button.
const int D_timer = 30; // Defines the default lenght (30 seconds) of the timer.
bool exit_loop = false; // Defines if the sourcefiles main should exit loop and end the program.

std::array<std::string, 256> applications; // Contains the applications that will be closed.

/* Refreshes variables upon updating the config file */
void RefreshConfig()
{
    YAML::Node config = YAML::LoadFile("config.yaml"); // Config file (config.yaml).

    enable_timer = config["EnableTimer"].as<bool>();
    if (config["Timer"].as<std::string>() == "null" || "NULL" ||"default" || "DEFAULT" || "") { timer = D_timer; }
    else { timer = config["Timer"].as<int>(); }
    enable_closebutton = config["EnableCloseButton"].as<bool>();
    if (config["CloseButton"].as<std::string>() == "null" || "NULL" || "default" || "DEFAULT" || "") { close_button = D_close_button; }
    else { close_button = config["CloseButton"].as<int>(); }
    system_tray = config["SystemTray"].as<bool>();

    for (std::size_t i = 0; i < config["Applications"].size(); i++)
    {
        applications[i] = config["Applications"][i].as<std::string>();
    }
}

/* main */
int main()
{
    if (!allow_execution) { return 0; }

    RefreshConfig();
    GetFileLastWriteTime();

    for (int i = 0; i < 3; i++)
    {
        std::cout << file_date[i] << " ";
    }
    std::cout << "\n";
    for (int i = 0; i < 2; i++)
    {
        std::cout << file_time[i] << " ";
    }
    
    if (system_tray) { HideIntoSystemTray(); }
    if (!enable_closebutton && !enable_timer) { close_button = true; }

    while (!exit_loop)
    {
        if (enable_closebutton) { exit_loop = CloseButtonMain(close_button, applications); }
        if (enable_timer) { exit_loop = TimerMain(); }
    }

    return 0;
}