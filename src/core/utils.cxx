import utils;

//import <assert.h>;
//import <iostream>;

#include<Windows.h>

#include<iostream>
#include<assert.h>

import globals;

[[nodiscard]]
_Check_return_
bool utils::memory::detour32(
	_In_ std::uint8_t* const bypAddress,
	_In_ const std::uint8_t* const bypNewFunction,
	_In_ const std::size_t length
) noexcept
{
	assert(length >= 5u);

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
	return static_cast<const std::ptrdiff_t>(pTo - pFrom - 5u);
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

void utils::messagebox::errorA(
	_In_z_ const char* const cstrBody
) noexcept
{
	assert(cstrBody && cstrBody[NULL] != '\0');

	MessageBoxA(
		nullptr,
		"Error!",
		cstrBody,
		(MB_OK | MB_ICONERROR)
	);
}

void utils::messagebox::errorW(
	_In_z_ const wchar_t* const wcstrBody
) noexcept
{
	assert(wcstrBody && wcstrBody[NULL] != L'\0');

	MessageBoxW(
		nullptr,
		L"Error!",
		wcstrBody,
		(MB_OK | MB_ICONERROR)
	);
}

__declspec(noreturn) void utils::dll::eject(
	_In_ void* const vpInstDLL,
	_In_ unsigned long int dwExitCode
) noexcept
{
	(*globals::function::pointer::pPopupMessage)("Exitting...");

	FreeLibraryAndExitThread(static_cast<const HMODULE>(vpInstDLL), dwExitCode);
}