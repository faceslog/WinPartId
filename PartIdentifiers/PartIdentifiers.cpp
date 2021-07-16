// PartIdentifiers.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Faceslog

// Detecting the type of disk:
// https://docs.microsoft.com/en-us/windows/win32/fileio/basic-and-dynamic-disks?redirectedfrom=MSDN#detecting-the-type-of-disk
// Naming a volume:
// https://docs.microsoft.com/en-us/windows/win32/fileio/naming-a-volume
// Partition GUID:
// https://en.wikipedia.org/wiki/GUID_Partition_Table

#include "WinDisk.h"

int main()
{
    std::wcout << std::endl;
    std::wcout << "Faceslog - Disk Helper" << std::endl;
    std::wcout << "Email: faces@faceslog.com" << std::endl;
    std::wcout << "Discord: faceslog#6851" << std::endl;
    std::wcout << "Github: www.github.com/faceslog" << std::endl;
    std::wcout << std::endl;

    for (auto& drive : WinDisk::GetValidDrives())
    {
        std::wcout << std::endl;
        std::wcout << "=======================================================" << std::endl;
        std::wcout << L"Drive: " << drive << std::endl;

        std::wstring vGuid = WinDisk::GetVolumeGuid(drive);
        std::wcout << L"GUID: " << vGuid << std::endl;
        
        for (auto& nb : WinDisk::GetDiskNumbers(vGuid))
        {
            std::wcout << "Disk Numbers: " << nb << std::endl;
           
            auto part = WinDisk::GetPartList(nb);

            for (size_t i{ 0 }; i < part.size(); i++)
            {
                std::wcout << "-----------------------" << std::endl;
                std::wcout << "Partition " << (i + 1) << ':' << std::endl;
                WinDisk::DisplayPartDetail(part.at(i));
                
            }

        }
        std::wcout << "=======================================================" << std::endl;
        std::wcout << std::endl;
    }

    std::cout << "Press any key to exit this window ..." << std::endl;
    (void) std::getchar();

    return EXIT_SUCCESS;
}

