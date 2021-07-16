// PartIdentifiers.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// Naming a volume:
// https://docs.microsoft.com/en-us/windows/win32/fileio/naming-a-volume

#include <iostream>
#include "WinDisk.h"


int main()
{
    
    for (auto& drive : WinDisk::GetValidDrives())
    {
        std::wcout << drive << std::endl;
    }

    /*
    DWORD nBufferLength = MAX_PATH;
    wchar_t szLogicalDrives[MAX_PATH] = { 0 };
    wchar_t szVolumeName[MAX_PATH] = { 0 };

    // MSDN: 
    // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getlogicaldrivestringsw
    // GetLogicalDriveStrings();
    DWORD dwResult = GetLogicalDriveStrings(nBufferLength, szLogicalDrives);

    if (dwResult > 0 && dwResult <= nBufferLength)
    {
        wchar_t* szSingleDrive = szLogicalDrives;
        while (*szSingleDrive)
        {
            std::wcout << szSingleDrive << std::endl;

            // Pass the mount point to GetVolumeNameForVolumeMountPoint to obtain the volume name.
            if (GetVolumeNameForVolumeMountPoint(szSingleDrive, szVolumeName, nBufferLength))
            {
                std::wcout << szVolumeName << std::endl;
                // Delete the trailing backslash to call CreateFile():
                szVolumeName[wcslen(szVolumeName) - 1] = '\0';

                HANDLE hVolume = CreateFile(szVolumeName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
                if (hVolume == INVALID_HANDLE_VALUE)
                {
                    std::cout << "CreateFile() Failed: " << GetLastError() << std::endl;
                    CloseHandle(hVolume);
                    return EXIT_FAILURE;
                }

                std::cout << "CreateFile Success" << std::endl;
                CloseHandle(hVolume);
            }

            // get the next drive
            szSingleDrive += wcslen(szSingleDrive) + 1;
        }
    }
    */
    return EXIT_SUCCESS;
}

