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

namespace WinDisk
{
	std::vector<std::wstring> GetValidDrives();

	std::wstring GetVolumeGuid(const std::wstring& mountPoint);

	std::vector<DWORD> GetDiskNumbers(std::wstring szVolumeName);

	std::vector<PARTITION_INFORMATION_EX> GetPartList(DWORD diskNumber);

	std::wstring GUIDToWstring(const GUID& guid);
}