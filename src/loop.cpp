#include <windows.h>
#include <array>
#include <string>
#include <processthreadsapi.h>
#include <tlhelp32.h>
#include <iostream>
#include "loop.h"
#include "yaml-cpp/yaml.h"

/* Refreshes variables upon updating the config file */
void RefreshConfig()
{
    YAML::Node config = YAML::LoadFile("config.yaml"); // Config file (config.yaml).

    enable_timer = config["EnableTimer"].as<bool>();
    enable_closebutton = config["EnableCloseButton"].as<bool>();
    system_tray = config["SystemTray"].as<bool>();

    if (config["CloseButton"].IsNull())
    {
        std::cout << "CloseButton is null\n";
        std::cout << "Fallback to default close button\n";
        close_button = D_close_button;
    }
    else
    {
        try
        {
            close_button = config["CloseButton"].as<int>();
        }
        catch(const YAML::TypedBadConversion<int>& e)
        {
            std::cerr << "TypedBadConversion<int>: " << e.what() << '\n';
            std::cerr << "Fallback to default close button\n";
            close_button = D_close_button;
        }
        
    }

    if (config["Timer"].IsNull())
    {
        std::cout << "Timer is null\n";
        std::cout << "Fallback to default timer\n";
        timer = D_timer;
    }
    else
    {
        try
        {
            timer = config["Timer"].as<int>();
        }
        catch(const YAML::TypedBadConversion<int>& e)
        {
            std::cerr << "TypedBadConversion<int>: " << e.what() << '\n';
            std::cerr << "Fallback to default time\n";
            timer = D_timer;
        }
        
    }

    for (std::size_t i = 0; i < config["Applications"].size(); i++)
    {
        applications[i] = config["Applications"][i].as<std::string>();
    }

    //std::cout << "e_timer: " << enable_timer << "\n";
    //std::cout << "timer: " << timer << "\n";
    //std::cout << "e_closebutton: " << enable_closebutton << "\n";
    //std::cout << "closebutton: " << close_button << "\n";
    //std::cout << "systemtray: " << system_tray << "\n";
    //std::cout << "Applications:\n";
    //for (std::size_t i = 0; i < config["Applications"].size(); i++)
    //{
    //    std::cout << applications[i] << "\n";
    //}
    //std::cout << "Config file updated\n";

    return;
}
/* Returns bool whether the program is running or not. */
bool isProcessRunning(const char* processName, DWORD& processId)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(entry);

    if (Process32First(snapshot, &entry))
    {
        do
        {
            if (_stricmp(entry.szExeFile, processName) == 0)
            {
                processId = entry.th32ProcessID;
                CloseHandle(snapshot);
                return true;
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return false;
}
/* Returns DWORD/processId of the program that is running. Returns -1 in error. */
DWORD returnProcessId(const char* processName, DWORD& processId)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(entry);

    if (Process32First(snapshot, &entry))
    {
        do
        {
            if (_stricmp(entry.szExeFile, processName) == 0)
            {
                processId = entry.th32ProcessID;
                CloseHandle(snapshot);
                return processId;
            }
        } while (Process32Next(snapshot, &entry));
    }

    return -1;
}
/* Gets "config.yaml" file latest update date and time then inserts them into file_date & file_time. */
void GetFileLastWriteTime()
{
    HANDLE hFile = CreateFileA(
        "config.yaml",
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    FILETIME ftCreate, ftAccess, ftWrite;

    if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
    {
        SYSTEMTIME stUTC, stLocal;
        FileTimeToSystemTime(&ftWrite, &stUTC);
        SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

        file_date = { stLocal.wYear, stLocal.wMonth, stLocal.wDay }; // File date (Year, Month, Day)
        file_time = { stLocal.wHour, stLocal.wMinute, stLocal.wSecond}; // File time (Hour, Minute, Second)
        CloseHandle(hFile);
        //std::cout << "File date: " << file_date[0] << "-" << file_date[1] << "-" << file_date[2] << "\n";
        //std::cout << "File time: " << file_time[0] << ":" << file_time[1] << "-" << file_time[2] << "\n";
        //std::cout << "File time updated\n";
        return;
    }

    else
    {
        std::cout << "Error getting file time\n";
        CloseHandle(hFile);
        return;
    }
}
/* Returns bool whether the current date/time and "config.yaml" update date/time is equal(false) (minutes-1) or not(true) */
bool ReturnDateTimeValidy()
{
    //std::cout << "Checking file date/time validy\n";
    GetFileLastWriteTime(); // Update file_date and file_time

    SYSTEMTIME st;
    GetLocalTime(&st);
    std::array<int, 3> current_date = { st.wYear, st.wMonth, st.wDay }; // Current date (Year, Month, Day)
    std::array<int, 3> current_time = { st.wHour, st.wMinute, st.wSecond }; // Current time (Hour, Minute, Second)

    if (current_date[0] > file_date[0]) // Year
        return true;

    else if (current_date[0] == file_date[0]) // Year
    {
        if (current_date[1] > file_date[1]) // Month
            return true;

        else if (current_date[1] == file_date[1]) // Month
        {
            if (current_date[2] > file_date[2]) // Day
                return true;

            else if (current_date[2] == file_date[2]) // Days
            {
                 /* Compare times if the dates are the same */
                if (current_time[0] > file_time[0]) // Hour
                    return true;

                else if (current_time[0] == file_time[0]) // Hour
                {
                    if (current_time[1] > file_time[1]) // Minute
                        return true;

                    else if (current_time[1] == file_time[1]) // Minute
                    {
                        if (current_time[2] == file_time[2] || current_time[2] <= file_time[2] + 3) // Second
                        {
                            return false;
                        }
                        else
                            return true;
                    }
                }
            }
        }
    }

    return false;
}

bool CloseButtonMain()
{
    DWORD events;
    INPUT_RECORD inputRecord;
    HANDLE keyBoard = GetStdHandle(STD_INPUT_HANDLE);

    while (true)
    {
        if (PeekConsoleInput(keyBoard, &inputRecord, 1, &events) != 0 && events > 0)
        {
            if (ReadConsoleInput(keyBoard, &inputRecord, 1, &events) != 0)
            {
                if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown)
                {
                    if (inputRecord.Event.KeyEvent.wVirtualKeyCode == close_button)
                    {
                        for (int i = 0; i < 256; i++)
                        {
                            if (applications[i].empty()) 
                            {
                                std::cout << "array is empty\n";
                                break;
                            }
                            else 
                            {
                                std::cout << "array is not empty\n";
                                std::cout << "Process name: " << applications[i].data() << "\n";
                            }

                            const char* processName = applications[i].data();
                            DWORD processId = 0;

                            if (isProcessRunning(processName, processId))
                            {
                                processId = returnProcessId(processName, processId);
                                if (processId == -1)
                                {
                                    std::cout << "processId returned error.\n";
                                    break;
                                }

                                HANDLE processHandle = OpenProcess(PROCESS_TERMINATE, false, processId);
                                if (processHandle)
                                {
                                    if (TerminateProcess(processHandle, 0))
                                    {
                                        std::cout << "Process terminated: " << processName << "\n";
                                        CloseHandle(processHandle);
                                        continue;
                                    }
                                    else
                                    {
                                        std::cout << "TerminateProcess failed\n";
                                        CloseHandle(processHandle);
                                        break;
                                    }
                                }
                                else
                                {
                                    std::cout << "OpenProcess failed\n";
                                    break;
                                }
                            }

                            else
                            {
                                std::cout << "Process not found: " << processName << "\n";
                            }
                        }
        
                        return false;
                    }
                }
            }
        }

        else
        {
            Sleep(300);
            if(!ReturnDateTimeValidy()) { RefreshConfig(); }
        }
    }

    return true;
}

bool TimerMain()
{
    std::cout << "timer selected\n";
    return true;
}