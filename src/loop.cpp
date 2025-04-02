#include <windows.h>
#include <array>
#include <string>
#include <processthreadsapi.h>
#include <tlhelp32.h>
#include <iostream>
#include "loop.h"

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

    return 'cerr';
}

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

        file_date[0] = stLocal.wYear;
        file_date[1] = stLocal.wMonth;
        file_date[2] = stLocal.wDay;
        file_time[0] = stLocal.wHour;
        file_time[1] = stLocal.wMinute;
        CloseHandle(hFile);
        return;
    }
    else
    {
        std::cout << "Error getting file time\n";
        CloseHandle(hFile);
        return;
    }
}

bool ReturnDateTimeValidy()
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    std::array<int, 3> current_date = { st.wYear, st.wMonth, st.wDay }; // Current date (Year, Month, Day)
    std::array<int, 2> current_time = { st.wHour, st.wMinute }; // Current time (Hour, Minute)

    // Compare dates
    if (current_date[0] > file_date[0]) // Year
        return true;

    else if (current_date[0] == file_date[0]) 
    {
        if (current_date[1] > file_date[1]) // Month
            return true;

        else if (current_date[1] == file_date[1]) 
        {
            if (current_date[2] > file_date[2]) // Day
                return true;

            else if (current_date[2] == file_date[2]) 
            {
                // Compare times if the dates are the same
                if (current_time[0] > file_time[0]) // Hour
                    return true;

                else if (current_time[0] == file_time[0]) 
                {
                    if (current_time[1] > file_time[1]) // Minute
                        return true;

                    else if (current_time[1] == file_time[1] || current_time[1] == file_time[1] - 1) 
                    {
                        // If minutes are the same or off by one, return false
                        return false;
                    }
                }
            }
        }
    }

    return false; // If none of the conditions are met, return false
}

void FileUpdate()
{
    std::cout << "File update\n";
}

bool CloseButtonMain(const int close_button, std::array<std::string, 256> applications)
{
    std::cout << "close_button selected\n";
    DWORD events;
    INPUT_RECORD inputRecord;
    HANDLE keyBoard = GetStdHandle(STD_INPUT_HANDLE);

    while (true) //if (GetAsyncKeyState(close_button) & 1)
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
        
                            return false;

                            const char* processName = applications[i].data();
                            DWORD processId = 0;

                            if (isProcessRunning(processName, processId))
                            {
                                processId = returnProcessId(processName, processId);
                                if (processId == 'cerr')
                                {
                                    std::cout << "processId returned error.\n";
                                    break;
                                }

                                HANDLE processHandle = OpenProcess(PROCESS_TERMINATE, false, processId);
                                if (processHandle)
                                {
                                    if (TerminateProcess(processHandle, 0))
                                    {
                                        CloseHandle(processHandle);
                                        return false;
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
                        }
                        std::cout << "Error happened!\n";
        
                        return false;
                    }
                }
            }
        }

        else
        {
            Sleep(300);
            if(!ReturnDateTimeValidy)
            {
                //FileUpdate();
                std::cout << "File update\n";
            }
        }
    }
    return true;
}

bool TimerMain()
{
    std::cout << "timer selected\n";
    return true;
}