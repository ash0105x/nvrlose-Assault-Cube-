import utils;

#include<Windows.h>
#include<TlHelp32.h>

#include<iostream>

import<tchar.h>;
import<cassert>;

import globals;
import signatures;

[[nodiscard]]
_Must_inspect_result_
_Ret_maybenull_
_Success_(return != nullptr)
std::uint8_t* const utils::memory::findSignature(
	_In_ const HMODULE hModule,
	_In_reads_(patternLength) const SignatureData_t& signatureData,
	_In_ const std::size_t patternLength
) noexcept
{
#ifdef _DEBUG
	const BYTE* const& cstrPattern = std::get<SIGNATURE_DATA_INDEX::SIGNATURE_DATA_INDEX_PATTERN>(signatureData);
	const char* const& cstrMask = std::get<SIGNATURE_DATA_INDEX::SIGNATURE_DATA_INDEX_MASK>(signatureData);
	const std::size_t actualMaskLength = std::strlen(cstrMask);


	assert(
		hModule &&
		(
			reinterpret_cast<const std::uintptr_t>(hModule) >= (
				offsetof(IMAGE_DOS_HEADER, e_lfanew) +
				sizeof(IMAGE_DOS_HEADER::e_lfanew)
			)
		) &&
		patternLength > NULL &&
		reinterpret_cast<const std::uintptr_t>(hModule) >= patternLength &&
		cstrPattern && cstrPattern[NULL] != static_cast<const BYTE>('\0') &&
		cstrMask && cstrMask[NULL] != '\0' &&
		actualMaskLength == patternLength
	);

	for (std::size_t i = NULL; i < patternLength; ++i) {
		const char& cRefMaskValue = cstrMask[i];

		assert(cRefMaskValue == '?' || cRefMaskValue == 'x');
	}
#endif // _DEBUG

	const std::uint8_t* const comparableModuleEnd = reinterpret_cast<const std::uint8_t* const>(
		reinterpret_cast<const std::uint8_t* const>(hModule) +
		reinterpret_cast<const IMAGE_NT_HEADERS* const>(
			reinterpret_cast<const std::uint8_t* const>(hModule) +
			reinterpret_cast<const IMAGE_DOS_HEADER* const>(hModule)->e_lfanew
		)->OptionalHeader.SizeOfImage -
		patternLength
	);

	for (std::uint8_t* pBase = reinterpret_cast<std::uint8_t* const>(hModule); pBase < comparableModuleEnd; ++pBase) {
		for (std::uintptr_t patternComparisonPointer = NULL; patternComparisonPointer < patternLength; ++patternComparisonPointer) {
			if (
				'x' == std::get<SIGNATURE_DATA_INDEX::SIGNATURE_DATA_INDEX_MASK>(signatureData)[patternComparisonPointer] &&
				pBase[patternComparisonPointer] != std::get<SIGNATURE_DATA_INDEX::SIGNATURE_DATA_INDEX_PATTERN>(signatureData)[patternComparisonPointer]
			)
			{
				goto invalidPattern;
			}
		}

		return pBase;

	invalidPattern:
		continue;
	}

	return nullptr;
}

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
			) >= (
				offsetof(MEMORY_BASIC_INFORMATION, Protect) +
				sizeof(MEMORY_BASIC_INFORMATION::Protect)
			)
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

void utils::memory::placeJump(
	_In_ std::uint8_t* const bypAddress,
	_In_ const std::uint8_t* const bypNewFunction
) noexcept
{
	assert(
		bypAddress &&
		utils::memory::isExecutableRegion(bypAddress) &&
		bypNewFunction &&
		utils::memory::isExecutableRegion(bypNewFunction)
	);

	const std::ptrdiff_t ptrRelativeOffset = utils::memory::calculateRelativeOffset(bypAddress, bypNewFunction);

	bypAddress[NULL] = utils::x86asm::JMP;
	*reinterpret_cast<DWORD* const>(bypAddress + 1u) = ptrRelativeOffset;
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

	utils::memory::placeJump(bypAddress, bypNewFunction);

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
	_In_ const utils::process::enumerateFunction_t& pRefEnumFunction,
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
	const std::array<const float, 4 * 4>& fArrRefModelViewProjectionMatrix = *globals::screen::pfArrModelViewProjectionMatrix;

	const float fClipCoordinateW = (
		vec3RefWorld.x * fArrRefModelViewProjectionMatrix[3u] +
		vec3RefWorld.y * fArrRefModelViewProjectionMatrix[7u] +
		vec3RefWorld.z * fArrRefModelViewProjectionMatrix[11u] +
		fArrRefModelViewProjectionMatrix[15u]
	);

	if (fClipCoordinateW < 0.1f) {
		return false;
	}

	const CVector2 vec2NormalizedDeviceCoordinates = CVector2{
		(
			vec3RefWorld.x * fArrRefModelViewProjectionMatrix[NULL] +
			vec3RefWorld.y * fArrRefModelViewProjectionMatrix[4u] +
			vec3RefWorld.z * fArrRefModelViewProjectionMatrix[8u] +
			fArrRefModelViewProjectionMatrix[12u]
		) / fClipCoordinateW,
		(
			vec3RefWorld.x * fArrRefModelViewProjectionMatrix[1u] +
			vec3RefWorld.y * fArrRefModelViewProjectionMatrix[5u] +
			vec3RefWorld.z * fArrRefModelViewProjectionMatrix[9u] +
			fArrRefModelViewProjectionMatrix[13u]
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