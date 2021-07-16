// PartIdentifiers.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Faceslog

// Detecting the type of disk:
// https://docs.microsoft.com/en-us/windows/win32/fileio/basic-and-dynamic-disks?redirectedfrom=MSDN#detecting-the-type-of-disk
// Naming a volume:
// https://docs.microsoft.com/en-us/windows/win32/fileio/naming-a-volume
// Partition GUID:
// https://en.wikipedia.org/wiki/GUID_Partition_Table

#include "WinDisk.h"

int DisplayMenu(const std::vector<std::wstring>& drives, int exit)
{
    std::wcout << std::endl;
    std::wcout << "Faceslog - Disk Helper" << std::endl;
    std::wcout << "Email: faces@faceslog.com" << std::endl;
    std::wcout << "Discord: faceslog#6851" << std::endl;
    std::wcout << "Github: www.github.com/faceslog" << std::endl;
    std::wcout << std::endl;

    std::wcout << "Select a Drive to Query Informations: " << std::endl;
    for (size_t i{ 0 }; i < drives.size(); i++)
    {
        std::wcout << '[' << i << "] - " << drives.at(i) << std::endl;
    }

    // Add supplementary options
    std::wcout << '[' << drives.size() << "] - " << "To Refresh Drive List" << std::endl;
    std::wcout << '[' << exit << "] - " << "To Exit The Program" << std::endl;

    int x;
    do
    {
        std::wcout << "Input: ";
        std::cin >> x;
        std::cout << std::endl;

    } while (x < 0 || x > exit);

    return x;
}

void DisplayDiskInfo(const std::wstring& drive)
{
    std::wcout << std::endl;
    std::wcout << L"============================ DISK INFO: ==========================" << std::endl;
    std::wcout << L"Drive: " << drive << std::endl;

    std::wstring vGuid = WinDisk::GetVolumeGuid(drive);
    // Check that the GUID is not empty
    if (!vGuid.empty())
    {
        std::wcout << L"GUID: " << vGuid << std::endl;
        for (auto& nb : WinDisk::GetDiskNumbers(vGuid))
        {
            std::wcout << L"Disk Numbers: " << nb << std::endl;

            auto part = WinDisk::GetPartList(nb);

            for (size_t i{ 0 }; i < part.size(); i++)
            {
                std::wcout << L"-----------------------" << std::endl;
                std::wcout << L"Partition " << (i + 1) << ':' << std::endl;
                WinDisk::DisplayPartDetail(part.at(i));

            }

        }
    }
    else
    {
        std::wcout << L"An error occured with that drive, did you unplugged it ?" << std::endl;
    }
        
    std::wcout << L"==================================================================" << std::endl;
    std::wcout << std::endl;
}

int main()
{    
    while (true)
    {
        auto drives = WinDisk::GetValidDrives();

        int exit = drives.size() + 1;
        auto result = DisplayMenu(drives, exit);

        // Refresh the drives list
        if (result == drives.size())
            continue;
        // Exit the program
        if (result == (exit)) 
            break;

        DisplayDiskInfo(drives.at(result));
    }
    
    std::wcout << "Thanks for using me :)" << std::endl;

    system("pause");

    return EXIT_SUCCESS;
}

