#pragma once

#include <Windows.h>
#include <string>
#include <vector>

namespace WinDisk
{
	std::vector<std::wstring> GetValidDrives();

	std::wstring GetVolumeGuid(const std::wstring& mountPoint);
}