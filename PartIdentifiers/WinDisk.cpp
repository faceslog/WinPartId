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
        // std::wcout << "[A] CreateFile() Failed: " << GetLastError() << std::endl;
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
            // std::wcout << "[A] DeviceIoControl() Failed: " << nError << std::endl;
            CloseHandle(hDevice);
            return diskNumbers;
        }

        // Allocate enough buffer space based of the value of NumberOfDiskExtents: 
        size_t size = offsetof(VOLUME_DISK_EXTENTS, Extents[vde.NumberOfDiskExtents]);
        std::vector<BYTE> buffer(size);

        if (!DeviceIoControl(hDevice, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, (void*)buffer.data(), size, &bytesReturned, NULL))
        {
            nError = GetLastError();
            // std::wcout << "[A] DeviceIoControl() Failed: " << nError << std::endl;
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

std::vector<PARTITION_INFORMATION_EX> WinDisk::GetPartList(DWORD diskNumber)
{
    std::vector<PARTITION_INFORMATION_EX> partitions;

    // Use the disk numbers to construct the disk paths, such as "\\?\PhysicalDriveX".
    std::wstring diskPath = L"\\\\?\\PhysicalDrive" + std::to_wstring(diskNumber);

    HANDLE hDevice = CreateFile(diskPath.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        // std::wcout << "[B] CreateFile() Failed: " << GetLastError() << std::endl;
        CloseHandle(hDevice);
        return partitions;
    }

    DRIVE_LAYOUT_INFORMATION_EX dli;
    DWORD bytesReturned = 0;

    if (!DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0, (void*)&dli, sizeof(dli), &bytesReturned, NULL))
    {
        // Check last error if not ERROR_INSUFFICIENT_BUFFER then return
        int nError = GetLastError();
        if (nError != ERROR_INSUFFICIENT_BUFFER)
        {
            // std::wcout << "[B] DeviceIoControl() Failed: " << nError << std::endl;
            CloseHandle(hDevice);
            return partitions;
        }

        // Allocate enough buffer space based of the value of Partition Count:
        size_t size = offsetof(DRIVE_LAYOUT_INFORMATION_EX, PartitionEntry[dli.PartitionCount]);
        std::vector<BYTE> buffer(size);

        if (!DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0, (void*)buffer.data(), size, &bytesReturned, NULL))
        {
            nError = GetLastError();
            // std::wcout << "[B] DeviceIoControl() Failed: " << nError << std::endl;
            CloseHandle(hDevice);
            return partitions;
        }

        // At this point we have a fully populated DRIVE_LAYOUT_INFORMATION_EX structure:
        const DRIVE_LAYOUT_INFORMATION_EX& result = *reinterpret_cast<const DRIVE_LAYOUT_INFORMATION_EX*>(buffer.data());

        for (size_t i{ 0 }; i < result.PartitionCount; i++)
        {
            partitions.push_back(result.PartitionEntry[i]);
        }
    }
    else
    {
        for (size_t i{ 0 }; i < dli.PartitionCount; i++)
        {
            partitions.push_back(dli.PartitionEntry[i]);
        }
    }
       
    CloseHandle(hDevice);

    return partitions;
}

std::wstring WinDisk::GUIDToWstring(const GUID& guid)
{
    wchar_t* guidString;
    HRESULT result = StringFromCLSID(guid, &guidString);

    if (result == E_OUTOFMEMORY)
    {
        CoTaskMemFree(guidString);
        return std::wstring();
    }
    
    std::wstring temp(guidString);
    CoTaskMemFree(guidString);

    return temp;    
}

void WinDisk::DisplayPartDetail(const PARTITION_INFORMATION_EX& part)
{
    // 1 Byte = 9.31E-10 GB
    // 1GB = 1073741824 Bytes
    if (part.PartitionLength.QuadPart < GB_IN_BYTE)
    {
        std::wcout << "Size: " << (part.PartitionLength.QuadPart / (MB_IN_BYTE)) << "MB" << std::endl;
    }
    else
    {
        std::wcout << "Size: " << (part.PartitionLength.QuadPart / (GB_IN_BYTE)) << "GB" << std::endl;
    }
    
    switch (part.PartitionStyle)
    {
        case PARTITION_STYLE_MBR:
        {
            // https://docs.microsoft.com/en-us/windows/win32/api/winioctl/ns-winioctl-partition_information_mbr
            std::wcout << "Style: MBR" << std::endl;
            switch (part.Mbr.PartitionType)
            {
                case PARTITION_ENTRY_UNUSED:
                    std::wcout << "Type: ENTRY_UNUSED" << std::endl;
                    break;
                case PARTITION_EXTENDED:
                    std::wcout << "Type: EXTENDED" << std::endl;
                    break;
                case PARTITION_FAT_12:
                    std::wcout << "Type: FAT_12" << std::endl;
                    break;
                case PARTITION_FAT_16:
                    std::wcout << "Type: FAT_16" << std::endl;
                    break;
                case PARTITION_FAT32:
                    std::wcout << "Type: FAT32" << std::endl;
                    break;
                case PARTITION_IFS:
                    std::wcout << "Type: IFS" << std::endl;
                    break;
                case PARTITION_LDM:
                    std::wcout << "Type: LDM" << std::endl;
                    break;
                case PARTITION_NTFT:
                    std::wcout << "Type: NTFT" << std::endl;
                    break;
                case VALID_NTFT:
                    std::wcout << "Type: VALID_NTFT" << std::endl;
                    break;
                default:
                    std::wcout << "Type: Invalid Type" << std::endl;
            }
            std::wcout << "BootIndicator: " << (part.Mbr.BootIndicator ? "true" : "false" ) << std::endl;
            std::wcout << "RecognizedPartition: " << (part.Mbr.RecognizedPartition ? "true" : "false") << std::endl;
            std::wcout << "Hidden Sectors: " << part.Mbr.HiddenSectors << std::endl;
            std::wcout << "PartitionId: " << WinDisk::GUIDToWstring(part.Mbr.PartitionId) << std::endl;
            break;
        }
        case PARTITION_STYLE_GPT:
        {
            // https://docs.microsoft.com/en-us/windows/win32/api/winioctl/ns-winioctl-partition_information_gpt
            std::wcout << "Style: GPT" << std::endl;
            std::wcout << "Type: " << WinDisk::GUIDToWstring(part.Gpt.PartitionType) << std::endl;
            std::wcout << "PartitionId: " << WinDisk::GUIDToWstring(part.Gpt.PartitionId) << std::endl;  
            break;
        }
        case PARTITION_STYLE_RAW:
        {
            std::wcout << "Style: RAW (Partition not formatted in either of the recognized formats—MBR or GPT)" << std::endl;
            break;
        }
        default:
            std::wcout << "Style: Invalid Style" << std::endl;
    }
}