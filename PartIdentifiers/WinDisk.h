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
}