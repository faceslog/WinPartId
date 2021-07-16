/* 
* Created by faceslog
* All functions were inspired:
* https://docs.microsoft.com/en-us/windows/win32/fileio/basic-and-dynamic-disks?redirectedfrom=MSDN#detecting-the-type-of-disk
* Some functions could be improved
* For Any Help - faces@faceslog.com
*/
#pragma once

#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>

#define GB_IN_BYTE 1073741824
#define MB_IN_BYTE 1048576

namespace WinDisk
{
	/**
	* @return a vector containing each valid drive in the system. Each string is a device name.
	* Call: GetLogicalDriveStringsW(): Fills a buffer with strings that specify valid drives in the system.
	* If it fails the vector will be empty
	**/
	std::vector<std::wstring> GetValidDrives();

	/**
	* @param mountPoint volume mount point (drive letter, volume GUID path, or mounted folder)
	* @return a volume GUID path for the volume that is associated with the specified volume mount point.
	* Call: GetVolumeNameForVolumeMountPointW()
	* If it fails the wstring will be empty
	**/
	std::wstring GetVolumeGuid(const std::wstring& mountPoint);

	/**
	* @param szVolumeName The name of the file or device to be created or opened. You may use either forward slashes (/) or backslashes (\) in this name.
	* @return a vector containing each disk numbers of the given volume name
	* Call: CreateFileW() and Use IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS
	* If it fails the vector will be empty
	**/
	std::vector<DWORD> GetDiskNumbers(std::wstring szVolumeName);

	/**
	* @param diskNumber, The disk number of a given volume
	* @return a vector containing each partition (PARTITION_INFORMATION_EX) of the given disk number
	* Call: CreateFileW() and Use IOCTL_DISK_GET_DRIVE_LAYOUT_EX
	* If it fails the vector will be empty
	**/
	std::vector<PARTITION_INFORMATION_EX> GetPartList(DWORD diskNumber);

	/**
	* @param guid, any GUID
	* @return the converted GUID as std::wstring
	* If it fails the wstring will be empty
	**/
	std::wstring GUIDToWstring(const GUID& guid);

	// Display Detail about a PARTITION_INFORMATION_EX structure
	void DisplayPartDetail(const PARTITION_INFORMATION_EX& part);
}