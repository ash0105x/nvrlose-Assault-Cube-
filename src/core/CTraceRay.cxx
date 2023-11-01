import CTraceRay;

import CVector3;
import globals;
import offsets;
import CPlayer;

#include"win32api.h"

import <cstdint>;

std::uint8_t* const g_ac_client_exe = reinterpret_cast<std::uint8_t* const>(GetModuleHandle(__TEXT("ac_client.exe")));

/*
* int ray@<eax>				ray is passed via register eax
*/
typedef void(__cdecl* const _traceLine_t)(
	_In_ const float fPositionFromX,
	_In_ const float fPositionFromY,
	_In_ const float fPositionFromZ,
	_In_ const float fPositionToX,
	_In_ const float fPositionToY,
	_In_ const float fPositionToZ,
	_In_opt_ const int pPlayer,
	_In_opt_ const bool bCheckPlayers,
	_In_opt_ const bool bSomeBoolSetToFalse
) noexcept;
static const _traceLine_t g_pTraceLineFn = reinterpret_cast<const _traceLine_t>(::g_ac_client_exe + offsets::ac_client_exe::function::TRACE_LINE);

/*
* bool bSkipCheckingSolids@<cl>	bSkipCheckingSolids is passed via register cl
* CPlayer* pPlayer@<eax>		pPlayer is passed via register eax
*/
typedef bool(__cdecl* const _isVisible_t)(
	_In_ const float fPositionFromX,
	_In_ const float fPositionFromY,
	_In_ const float fPositionFromZ,
	_In_ const float fPositionToX,
	_In_ const float fPositionToY,
	_In_ const float fPositionToZ
) noexcept;
static const _isVisible_t g_pIsVisibleFn = reinterpret_cast<const _isVisible_t>(::g_ac_client_exe + offsets::ac_client_exe::function::IS_VISIBLE);

/*
* float* pPlayer@<eax>		pPlayer is passed via register eax
* float* vec3Delta@<ebx>	vec3Delta is passed via register ebx
*/
typedef CROSSHAIR_ID(__cdecl* const _intersect_t)(
	_In_ const CVector3 vec3PositionFrom,
	_In_ const CVector3 vec3PositionTo
) noexcept;
static const _intersect_t g_pIntersectFn = reinterpret_cast<const _intersect_t>(::g_ac_client_exe + offsets::ac_client_exe::function::INTERSECT);

[[nodiscard]] void CTraceRay::traceLine(
	_In_ const CVector3& vec3PositionStart,
	_In_ const CVector3& vec3PositionEnd,
	_In_opt_ const CPlayer* const pTracer,
	_In_opt_ const bool bCheckPlayers,
	_In_opt_ const bool bSkipCheckingSolids
) noexcept
{
	__asm {
		push dword ptr[bSkipCheckingSolids]
		push dword ptr[bCheckPlayers]
		push dword ptr[pTracer]
		mov eax, dword ptr[vec3PositionEnd]
		push dword ptr[eax + 0x8u]
		push dword ptr[eax + 0x4u]
		push dword ptr[eax]
		mov eax, dword ptr[vec3PositionStart]
		push dword ptr[eax + 0x8u]
		push dword ptr[eax + 0x4u]
		push dword ptr[eax]
		push ecx
		mov ecx, dword ptr[this]
		lea eax, dword ptr[ecx]
		pop ecx
		call g_pTraceLineFn
		xor eax, eax
		add esp, 0x24u
	}
}

[[nodiscard]] bool CTraceRay::entityIsVisible(
	_In_ const CVector3& vec3PositionFrom,
	_In_ const CVector3& vec3PositionTo
) noexcept
{
	__asm {
		mov eax, dword ptr [vec3PositionTo]
		push dword ptr[eax + 0x8u]
		push dword ptr[eax + 0x4u]
		push dword ptr[eax]

		mov eax, dword ptr [vec3PositionFrom]
		push dword ptr[eax + 0x8u]
		push dword ptr[eax + 0x4u]
		push dword ptr[eax]

		xor eax, eax
		xor cl, cl

		call dword ptr[g_pIsVisibleFn]

		add esp, 0x18u
	}
}

[[nodiscard]] CROSSHAIR_ID CTraceRay::intersect(
	_In_ const CPlayer& refPlayer,
	_In_ const CVector3& vec3PositionFrom,
	_In_ const CVector3& vec3PositionTo
) noexcept
{
	CVector3& pvec3Coordinates = *reinterpret_cast<CVector3* const>(::g_ac_client_exe + 0x10A400u);

	const CVector3 vec3PreviousCoordinates = pvec3Coordinates;

	pvec3Coordinates = refPlayer.vec3EyePosition;

	__asm {
		mov eax, dword ptr[refPlayer]

		mov ecx, dword ptr[vec3PositionTo]
		mov edx, dword ptr[vec3PositionFrom]

		push ecx
		push edx

		xor ebx, ebx

		call dword ptr[g_pIntersectFn]

		add esp, 0x08u
	}

	pvec3Coordinates = vec3PreviousCoordinates;
}