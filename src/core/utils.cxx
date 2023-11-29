import utils;

//import <assert.h>;
//import <iostream>;

#include<Windows.h>

#include<iostream>
#include<assert.h>

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
		VirtualQuery(
			vpMemoryRegion,
			&memoryRegionInformation,
			sizeof(MEMORY_BASIC_INFORMATION)
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
	assert(bypAddress && bypNewFunction && length >= 5u);

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

	const DWORD dwRelativeOffset = utils::memory::calculateRelativeOffset(bypAddress, bypNewFunction);
	bypAddress[NULL] = utils::x86asm::jmp;
	*reinterpret_cast<DWORD* const>(bypAddress + 1u) = dwRelativeOffset;

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
_Must_inspect_result_
void* const utils::memory::findDMAAddress(
	void* const vpStart,
	const std::vector<std::ptrdiff_t>& offsets
) noexcept
{
	assert(vpStart);

	BYTE* bpDMAddress = static_cast<BYTE*>(vpStart);

	for (const std::ptrdiff_t& refOffset : offsets) {
		bpDMAddress = *reinterpret_cast<BYTE* const* const>(bpDMAddress);
		bpDMAddress += refOffset;
	}

	return bpDMAddress;
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

[[nodiscard]]
_Check_return_
_Ret_maybenull_
_Success_(return != nullptr)
const HMODULE utils::module::byName(
	_In_z_ const TCHAR* const tcstrName
) noexcept
{
	assert(tcstrName && tcstrName[NULL] != __TEXT('\0'));

	const HMODULE hModule = GetModuleHandle(tcstrName);

	(*globals::function::pointer::pPopupMessage)(
		std::_Is_character<std::remove_const<std::remove_reference<decltype(tcstrName[NULL])>::type>::type>::value ?
		"module %s -> 0x%p" :
		"module %ls -> 0x%p",
		tcstrName
	);

	return hModule;
}

[[nodiscard]]
_Check_return_
_Ret_maybenull_
_Success_(return != nullptr)
void* const utils::module::retrieveFunction(
	_In_z_ const TCHAR* const tcstrModuleName,
	_In_z_ const char* const cstrFunctionName
) noexcept
{
	assert(
		tcstrModuleName && tcstrModuleName[NULL] != __TEXT('\0')&&
		cstrFunctionName && cstrFunctionName[NULL]!= '\0'
	);

	const HMODULE hModule = utils::module::byName(tcstrModuleName);

	if (!hModule) {
		(*globals::function::pointer::pPopupMessage)("Invalid module!");
	}

	void* const vpFunction = GetProcAddress(hModule, cstrFunctionName);

	(*globals::function::pointer::pPopupMessage)(
		std::_Is_character<std::remove_const<std::remove_reference<decltype(tcstrModuleName[NULL])>::type>::type>::value ?
		"function %s - %s -> 0x%p" :
		"function %ls - %s -> 0x%p",
		tcstrModuleName, cstrFunctionName, vpFunction
	);

	return vpFunction;
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

	const float fHalfScreenWidth = static_cast<const float>(globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_WIDTH] / 2u);
	const float fHalfScreenHeight = static_cast<const float>(globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_HEIGHT] / 2u);

	vecRef2Screen = CVector2{
		(fHalfScreenWidth * vec2NormalizedDeviceCoordinates.x) +
		(fHalfScreenWidth + vec2NormalizedDeviceCoordinates.x),
		(fHalfScreenHeight * vec2NormalizedDeviceCoordinates.y) +
		(fHalfScreenHeight + vec2NormalizedDeviceCoordinates.y)
	};

	return true;
}

void utils::messagebox::error(
	_In_z_ const TCHAR* const tcstrBody
) noexcept
{
	assert(tcstrBody && tcstrBody[NULL] != __TEXT('\0'));

	MessageBox(
		nullptr,
		__TEXT("Error!"),
		tcstrBody,
		(MB_OK | MB_ICONERROR)
	);
}