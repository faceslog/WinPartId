#pragma once

#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>
#include <array>

namespace WinDisk
{
	std::vector<std::wstring> GetValidDrives();

	std::wstring GetVolumeGuid(const std::wstring& mountPoint);

	std::vector<DWORD> GetDiskNumbers(std::wstring szVolumeName);

	void PartitionList(DWORD diskNumber);
}