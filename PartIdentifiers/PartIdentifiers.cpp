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
    
    for (auto& drive : WinDisk::GetValidDrives())
    {
        std::wcout << std::endl;
        std::wcout << "=======================" << std::endl;
        std::wcout << L"Drive: " << drive << std::endl;

        std::wstring vGuid = WinDisk::GetVolumeGuid(drive);
        std::wcout << L"GUID: " << vGuid << std::endl;
        
        std::wcout << "Disk Numbers: ";
        for (auto& nb : WinDisk::GetDiskNumbers(vGuid))
        {
            std::wcout << nb << std::endl;
            
            auto part = WinDisk::GetPartList(nb);

            for (size_t i{ 0 }; i < part.size(); i++)
            {
                std::wcout << "=======================" << std::endl;
                std::wcout << "Partition " << (i + 1) << ':' << std::endl;

                switch (part.at(i).PartitionStyle)
                {
                case PARTITION_STYLE_MBR:
                {
                    std::wcout << "Style: MBR" << std::endl;
                    break;
                }
                case PARTITION_STYLE_GPT:
                {
                    std::wcout << "Style: GPT" << std::endl;
                    std::wcout << L"Type: " << WinDisk::GUIDToWstring(part.at(i).Gpt.PartitionType) << std::endl;
                    break;
                }
                case PARTITION_STYLE_RAW:
                {
                    std::wcout << "Style: RAW (Partition not formatted in either of the recognized formats—MBR or GPT)" << std::endl;
                    break;
                }
                default:
                    std::wcout << "Invalid Partition ..." << std::endl;
                }
            }

        }
        std::wcout << std::endl;
    }

    return EXIT_SUCCESS;
}

