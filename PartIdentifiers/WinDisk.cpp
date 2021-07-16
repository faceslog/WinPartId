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

std::wstring WinDisk::GetVolumeGuid(const std::wstring& mountPoint)
{
    DWORD nBufferLength = MAX_PATH;
    wchar_t szVolumeName[MAX_PATH] = { 0 };

    // Pass the mount point to GetVolumeNameForVolumeMountPoint to obtain the volume name.
    if (!GetVolumeNameForVolumeMountPoint(mountPoint.c_str(), szVolumeName, nBufferLength))
    {
        // std::wcout << "GetVolumeNameForVolumeMountPoint() Failed: " << GetLastError() << std::endl;
        // Return an empty string
        return std::wstring();
    }

    return std::wstring(szVolumeName);
}

std::vector<DWORD> WinDisk::GetDiskNumbers(std::wstring szVolumeName)
{
    std::vector<DWORD> diskNumbers;

    // Delete the trailing backslash to call CreateFile():
    szVolumeName.pop_back();

    HANDLE hDevice = CreateFile(szVolumeName.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        // std::wcout << "CreateFile() Failed: " << GetLastError() << std::endl;
        CloseHandle(hDevice);
        return diskNumbers;
    }

    VOLUME_DISK_EXTENTS vde = { 0 };
    DWORD bytesReturned = 0;

    if (!DeviceIoControl(hDevice, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, (void*)&vde, sizeof(vde), &bytesReturned, NULL))
    {
        // Check last error if not ERROR_MORE_DATA then return
        int nError = GetLastError();
        if (nError != ERROR_MORE_DATA)
        {
            // std::wcout << "DeviceIoControl() Failed: " << nError << std::endl;
            CloseHandle(hDevice);
            return diskNumbers;
        }

        // Allocate enough buffer space based of the value of NumberOfDiskExtents: 
        size_t size = offsetof(VOLUME_DISK_EXTENTS, Extents[vde.NumberOfDiskExtents]);
        std::vector<BYTE> buffer(size);

        if (!DeviceIoControl(hDevice, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, (void*)buffer.data(), size, &bytesReturned, NULL))
        {
            nError = GetLastError();
            // std::wcout << "DeviceIoControl() Failed: " << nError << std::endl;
            CloseHandle(hDevice);
            return diskNumbers;
        }

        // At this point we have a fully populated VOLUME_DISK_EXTENTS structure: 
        const VOLUME_DISK_EXTENTS& result = *reinterpret_cast<const VOLUME_DISK_EXTENTS*>(buffer.data());

        for (size_t i{ 0 }; i < result.NumberOfDiskExtents; i++)
        {
            diskNumbers.push_back(result.Extents[i].DiskNumber);
        }
    }
    else
    {
        // Call succeeded; vde is populated with single disk extent.
        for (size_t i{ 0 }; i < vde.NumberOfDiskExtents; i++)
        {
            diskNumbers.push_back(vde.Extents[i].DiskNumber);
        }
    }
    
    CloseHandle(hDevice);

    return diskNumbers;
}

void WinDisk::PartitionList(DWORD diskNumber)
{
    // Use the disk numbers to construct the disk paths, such as "\\?\PhysicalDriveX".
    std::wstring diskPath = L"\\\\?\\PhysicalDrive" + std::to_wstring(diskNumber);
    std::wcout << diskPath << std::endl;

    HANDLE hDevice = CreateFile(diskPath.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        std::wcout << "CreateFile() Failed: " << GetLastError() << std::endl;
        CloseHandle(hDevice);
        return;
    }

    DRIVE_LAYOUT_INFORMATION_EX dli;
    DWORD bytesReturned = 0;

    if (!DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0, (void*)&dli, sizeof(dli), &bytesReturned, NULL))
    {
        // Check last error if not ERROR_INSUFFICIENT_BUFFER then return
        int nError = GetLastError();
        if (nError != ERROR_INSUFFICIENT_BUFFER)
        {
            std::wcout << "1- DeviceIoControl() Failed: " << nError << std::endl;
            CloseHandle(hDevice);
            return;
        }

        // Allocate enough buffer space based of the value of Partition Count:
        size_t size = offsetof(DRIVE_LAYOUT_INFORMATION_EX, PartitionEntry[dli.PartitionCount]);
        std::vector<BYTE> buffer(size);

        if (!DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0, (void*)buffer.data(), size, &bytesReturned, NULL))
        {
            nError = GetLastError();
            std::wcout << "2- DeviceIoControl() Failed: " << nError << std::endl;
            CloseHandle(hDevice);
            return;
        }

        // At this point we have a fully populated DRIVE_LAYOUT_INFORMATION_EX structure:
        const DRIVE_LAYOUT_INFORMATION_EX& result = *reinterpret_cast<const DRIVE_LAYOUT_INFORMATION_EX*>(buffer.data());
        std::wcout << "2- Nb of Partitions: " << result.PartitionCount << std::endl;
        for (size_t i{ 0 }; i < result.PartitionCount; i++)
        {
            std::wcout << L"2- Partition " << (i + 1) << " :" << std::endl;

            switch (result.PartitionEntry[i].PartitionStyle)
            {
            case PARTITION_STYLE_MBR:
            {
                std::wcout << "2- MBR Partition ..." << std::endl;
                break;
            }
            case PARTITION_STYLE_GPT:
            {
                std::wcout << "2- GPT Partition ..." << std::endl;
                wchar_t* guidString;
                StringFromCLSID(result.PartitionEntry[i].Gpt.PartitionType, &guidString);
                std::wcout << L"2- Type: " << guidString << std::endl;
                CoTaskMemFree(guidString);
                break;
            }
            case PARTITION_STYLE_RAW:
                std::wcout << "2- RAW Partition ..." << std::endl;
                break;
            default:
                std::wcout << "2- Invalid Partition ..." << std::endl;
            }
        }

    }
    else
    {
        // Single Result:
        std::wcout << "Nb of Partitions: " << dli.PartitionCount << std::endl;

        for (size_t i{ 0 }; i < dli.PartitionCount; i++)
        {
            std::wcout << L"Partition " << (i + 1) << " :" << std::endl;

            switch (dli.PartitionEntry[i].PartitionStyle)
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
                StringFromCLSID(dli.PartitionEntry[i].Gpt.PartitionType, &guidString);
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
                
    CloseHandle(hDevice);
}