#pragma once
#include <array>
#include <string>

inline std::array<int, 3> file_date; // "config.yaml" File date (Year, Month, Day)
inline std::array<int, 3> file_time; // "config.yaml" File time (Hour, Minute, Second)

inline bool enable_timer; // Defines if the timer will be enabled.
inline int timer; // Defines in seconds how long the closing will wait. (Default 30 seconds)
inline bool enable_closebutton; // Defines if the close button will be enabled. (Set to true as default if both enable_timer/button are false)
inline int close_button; // Defines the button that will close app`s. (Default Delete (0x2E) key)
inline bool system_tray; // Defines if the software will be hidden into system tray or shown in cmd format.

inline const int D_close_button = 0x2E; // Defines the default key (Delete) for close button.
inline const int D_timer = 30; // Defines the default lenght (30 seconds) of the timer.
inline bool exit_loop = false; // Defines if the sourcefiles main should exit loop and end the program.

inline std::array<std::string, 256> applications; // Contains the applications that will be closed.

/* Main function for close button loop. */
bool CloseButtonMain();
/* Main function for timer loop. */
bool TimerMain();
/* Takes the 'config.yaml' last update date and time and insets it into file_date & file_time. */
void GetFileLastWriteTime();
/* Refreshes variables upon updating the config file */
void RefreshConfig();