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
        std::wcout << std::endl;
        std::wcout << L"Drive: " << drive << std::endl;

        std::wstring vGuid = WinDisk::GetVolumeGuid(drive);
        std::wcout << L"GUID: " << vGuid << std::endl;
        
        std::cout << "Disk Numbers: ";
        for (auto& nb : WinDisk::GetDiskNumbers(vGuid))
        {
            std::cout << nb << ' ';
        }
        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}

