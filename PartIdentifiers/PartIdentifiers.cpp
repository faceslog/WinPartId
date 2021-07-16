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
        std::wcout << L"Drive: " << drive << std::endl;

        std::wstring vGuid = WinDisk::GetVolumeGuid(drive);
        std::wcout << L"GUID: " << vGuid << std::endl;
        
        std::cout << "Disk Numbers: ";
        for (auto& nb : WinDisk::GetDiskNumbers(vGuid))
        {
            std::cout << nb << std::endl;
            
            for (auto& part : WinDisk::GetPartList(nb))
            {
                switch (part.PartitionStyle)
                {
                case PARTITION_STYLE_MBR:
                {
                    std::wcout << "MBR Partition ..." << std::endl;
                    break;
                }
                case PARTITION_STYLE_GPT:
                {
                    std::wcout << "GPT Partition ..." << std::endl;
                    wchar_t* guidString;
                    StringFromCLSID(part.Gpt.PartitionType, &guidString);
                    std::wcout << L"Type: " << guidString << std::endl;
                    CoTaskMemFree(guidString);
                    break;
                }
                case PARTITION_STYLE_RAW:
                    std::wcout << "RAW Partition ..." << std::endl;
                    break;
                default:
                    std::wcout << "Invalid Partition ..." << std::endl;
                }
            }
        }
        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}

