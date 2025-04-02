#pragma once
#include <array>
#include <string>

inline std::array<int, 3> file_date; // File date (Year, Month, Day)
inline std::array<int, 2> file_time; // File time (Hour, Minute)

/* Main function for close button loop. */
bool CloseButtonMain(const int close_button, std::array<std::string, 256> applications);
/* Main function for timer loop. */
bool TimerMain();
/* Takes the 'config.yaml' last update date and time and insets it into file_date & file_time. */
void GetFileLastWriteTime();