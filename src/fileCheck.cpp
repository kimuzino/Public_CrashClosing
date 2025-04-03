#include <filesystem>
#include <fstream>
#include <iostream>

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
            file.close();
        }
        else
        {
            std::cerr << "Failed to create config.yaml\n";
        }
    }
}