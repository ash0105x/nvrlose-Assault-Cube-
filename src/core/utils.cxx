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