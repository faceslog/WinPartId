#include "WinDisk.h"

std::vector<std::wstring> WinDisk::GetValidDrives()
{
    DWORD nBufferLength = MAX_PATH;
    wchar_t szLogicalDrives[MAX_PATH] = { 0 };
    std::vector<std::wstring> drives;

    DWORD dwResult = GetLogicalDriveStrings(nBufferLength, szLogicalDrives);

    // Check if no error happend
    if (dwResult > 0 && dwResult <= nBufferLength)
    {
        wchar_t* szSingleDrive = szLogicalDrives;
        while (*szSingleDrive)
        {
            drives.push_back(szSingleDrive);
            // get the next drive
            szSingleDrive += wcslen(szSingleDrive) + 1;
        }
    }

    return drives;
}