import utils;

#include<Windows.h>
#include<TlHelp32.h>

#include<iostream>

import<tchar.h>;
import<cassert>;

import globals;

[[nodiscard]]
_Check_return_
_Success_(return == true)
bool utils::memory::isExecutableRegion(
	_In_opt_ const void* const vpMemoryRegion
) noexcept
{
	assert(vpMemoryRegion);

	MEMORY_BASIC_INFORMATION memoryRegionInformation = MEMORY_BASIC_INFORMATION{ };
	RtlZeroMemory(&memoryRegionInformation, sizeof(MEMORY_BASIC_INFORMATION));

	return(
		(
			(
				VirtualQuery(
					vpMemoryRegion,
					&memoryRegionInformation,
					sizeof(MEMORY_BASIC_INFORMATION)
				)
			) >= (offsetof(MEMORY_BASIC_INFORMATION, Protect) + sizeof(MEMORY_BASIC_INFORMATION::Protect))
		) &&
		memoryRegionInformation.Protect &
		(
			PAGE_EXECUTE |
			PAGE_EXECUTE_READ |
			PAGE_EXECUTE_READWRITE |
			PAGE_EXECUTE_WRITECOPY
		)
	);
}

[[nodiscard]]
_Check_return_
_Success_(return == true)
bool utils::memory::detour32(
	_In_ std::uint8_t* const bypAddress,
	_In_ const std::uint8_t* const bypNewFunction,
	_In_ const std::size_t length
) noexcept
{
	assert(
		bypAddress &&
		utils::memory::isExecutableRegion(bypAddress) &&
		bypNewFunction &&
		utils::memory::isExecutableRegion(bypNewFunction) &&
		length >= 5u
	);

	DWORD dwPreviousProtection = NULL;
	if (
		!VirtualProtect(
			bypAddress,
			length,
			PAGE_EXECUTE_READWRITE,
			&dwPreviousProtection
		)
	)
	{
		return false;
	}

	const std::ptrdiff_t ptrRelativeOffset = utils::memory::calculateRelativeOffset(bypAddress, bypNewFunction);

	bypAddress[NULL] = utils::x86asm::jmp;
	*reinterpret_cast<DWORD* const>(bypAddress + 1u) = ptrRelativeOffset;

	DWORD dwTempProtection = NULL;
	VirtualProtect(
		bypAddress,
		length,
		dwPreviousProtection,
		&dwTempProtection
	);

	return true;
}

[[nodiscard]]
std::ptrdiff_t utils::memory::calculateRelativeOffset(
	_In_ const std::uint8_t* const pFrom,
	_In_ const std::uint8_t* const pTo
) noexcept
{
	assert(pFrom && pTo);

	return static_cast<const std::ptrdiff_t>(pTo - pFrom - 5u);
}

_Check_return_opt_
_Success_(return == true)
bool utils::process::enumerate(
	_In_ bool(* const& pRefEnumFunction)(_In_ const PROCESSENTRY32& refProcessInfo32, _In_opt_ void* const vpExtraParameter) noexcept,
	_In_opt_ void* const vpExtraParameter
) noexcept
{
	assert(pRefEnumFunction);

	const HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (INVALID_HANDLE_VALUE == hProcessSnapshot) {
		return false;
	}

	PROCESSENTRY32 processInfo32 = PROCESSENTRY32{ };
	RtlZeroMemory(&processInfo32, sizeof(PROCESSENTRY32));
	processInfo32.dwSize = sizeof(PROCESSENTRY32);

	const auto safeExit = [&hProcessSnapshot](const bool bReturnValue) noexcept -> bool {
		CloseHandle(hProcessSnapshot);

		return bReturnValue;
	};

	if (!Process32First(hProcessSnapshot, &processInfo32)) {
		return safeExit(false);
	}

	do {
		if (!((*pRefEnumFunction)(processInfo32, vpExtraParameter))) {
			break;
		}
	} while (Process32Next(hProcessSnapshot, &processInfo32));

	return safeExit(true);
}

[[nodiscard]]
_Check_return_
_Success_(return == true)
bool utils::process::isRunning(
	_In_z_ const TCHAR* const tcstrProcessName
) noexcept
{
	assert(
		tcstrProcessName &&
		tcstrProcessName[NULL] != __TEXT('\0') &&
		_tcslen(tcstrProcessName) < sizeof(PROCESSENTRY32::szExeFile)
	);

	typedef struct ProcessInformation {
		const TCHAR* const& tcstrRefProcessName = nullptr;
		bool bIsRunning = false;
	}ProcessInformation_t;

	ProcessInformation_t processInformation = ProcessInformation_t{
		tcstrProcessName,
		false
	};

	utils::process::enumerate(
		[](_In_ const PROCESSENTRY32& refProcessInfo32, _In_opt_ void* const vpExtraParameter) noexcept -> bool {
			ProcessInformation_t& refProcessInformation = *static_cast<ProcessInformation_t* const>(vpExtraParameter);

			if (NULL != _tcscmp(refProcessInformation.tcstrRefProcessName, refProcessInfo32.szExeFile)) {
				return true;
			}

			refProcessInformation.bIsRunning = true;
			return false;
		},
		&processInformation
	);

	return processInformation.bIsRunning;
}

[[nodiscard]]
_Check_return_
_Ret_maybenull_z_
_Ret_z_
_Success_(return != nullptr)
const TCHAR* utils::process::name(
	_In_ const DWORD dwId
) noexcept
{
	assert(dwId);

	typedef struct ProcessInformation {
		const DWORD& dwRefId = NULL;
		const TCHAR* tcstrName = nullptr;
	}ProcessInformation_t;

	ProcessInformation_t processInformation = ProcessInformation_t{ dwId };

	utils::process::enumerate(
		[](_In_ const PROCESSENTRY32& refProcessInfo32, _In_opt_ void* const vpExtraParameter) noexcept -> bool {
			ProcessInformation_t& refProcessInformation = *static_cast<ProcessInformation_t* const>(vpExtraParameter);

			if (refProcessInfo32.th32ProcessID != refProcessInformation.dwRefId) {
				return true;
			}

			refProcessInformation.tcstrName = refProcessInfo32.szExeFile;

			return false;
		},
		&processInformation
	);

	return processInformation.tcstrName;
}

bool utils::math::worldToScreen(
	_In_ const CVector3& vec3RefWorld,
	_Out_ CVector2& vecRef2Screen
) noexcept
{
	const float fClipCoordinateW = (
		vec3RefWorld.x * globals::screen::pModelViewProjectionMatrix[3u] +
		vec3RefWorld.y * globals::screen::pModelViewProjectionMatrix[7u] +
		vec3RefWorld.z * globals::screen::pModelViewProjectionMatrix[11u] +
		globals::screen::pModelViewProjectionMatrix[15u]
	);

	if (fClipCoordinateW < 0.1f) {
		return false;
	}

	const CVector2 vec2NormalizedDeviceCoordinates = CVector2{
		(
			vec3RefWorld.x * globals::screen::pModelViewProjectionMatrix[NULL] +
			vec3RefWorld.y * globals::screen::pModelViewProjectionMatrix[4u] +
			vec3RefWorld.z * globals::screen::pModelViewProjectionMatrix[8u] +
			globals::screen::pModelViewProjectionMatrix[12u]
		) / fClipCoordinateW,
		(
			vec3RefWorld.x * globals::screen::pModelViewProjectionMatrix[1u] +
			vec3RefWorld.y * globals::screen::pModelViewProjectionMatrix[5u] +
			vec3RefWorld.z * globals::screen::pModelViewProjectionMatrix[9u] +
			globals::screen::pModelViewProjectionMatrix[13u]
		) / fClipCoordinateW
	};

	const float fHalfScreenWidth = static_cast<const float>(globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_WIDTH]) / 2.f;
	const float fHalfScreenHeight = static_cast<const float>(globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_HEIGHT]) / 2.f;

	vecRef2Screen = CVector2{
		(fHalfScreenWidth * vec2NormalizedDeviceCoordinates.x) +
		(fHalfScreenWidth + vec2NormalizedDeviceCoordinates.x),
		(fHalfScreenHeight * vec2NormalizedDeviceCoordinates.y) +
		(fHalfScreenHeight + vec2NormalizedDeviceCoordinates.y)
	};

	return true;
}