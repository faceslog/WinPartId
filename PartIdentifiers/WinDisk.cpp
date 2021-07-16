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
        // std::cout << "GetVolumeNameForVolumeMountPoint() Failed: " << GetLastError() << std::endl;
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

    HANDLE hVolume = CreateFile(szVolumeName.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (hVolume == INVALID_HANDLE_VALUE)
    {
        // std::cout << "CreateFile() Failed: " << GetLastError() << std::endl;
        CloseHandle(hVolume);
        return diskNumbers;
    }

    // From: https://stackoverflow.com/questions/33327177/how-to-call-deviceiocontrol-to-retrieve-the-amount-of-memory-it-needs
    VOLUME_DISK_EXTENTS vde = { 0 };
    DWORD bytesReturned = 0;

    if (!DeviceIoControl(hVolume, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, (void*)&vde, sizeof(vde), &bytesReturned, NULL))
    {
        // Check last error if not ERROR_MORE_DATA then return
        int nError = GetLastError();
        if (nError != ERROR_MORE_DATA)
        {
            // std::cout << "DeviceIoControl() Failed: " << nError << std::endl;
            CloseHandle(hVolume);
            return diskNumbers;
        }

        // Allocate enough buffer space based of the value of NumberOfDiskExtents: 
        size_t size = offsetof(VOLUME_DISK_EXTENTS, Extents[vde.NumberOfDiskExtents]);
        std::vector<BYTE> buffer(size);

        if (!DeviceIoControl(hVolume, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, (void*)buffer.data(), size, &bytesReturned, NULL))
        {
            nError = GetLastError();
            // std::cout << "DeviceIoControl() Failed: " << nError << std::endl;
            CloseHandle(hVolume);
            return diskNumbers;
        }

        // At this point we have a fully populated VOLUME_DISK_EXTENTS structure: 
        //const VOLUME_DISK_EXTENTS& result = *reinterpret_cast<const VOLUME_DISK_EXTENTS*>(buffer.data());
        vde = *reinterpret_cast<const VOLUME_DISK_EXTENTS*>(buffer.data());
    }
    
    // std::cout << "NumberOfDiskExtents: " << vde.NumberOfDiskExtents << std::endl;

    for (size_t i{ 0 }; i < vde.NumberOfDiskExtents; i++)
    {
        diskNumbers.push_back(vde.Extents[i].DiskNumber);
    } 

    CloseHandle(hVolume);

    return diskNumbers;
}