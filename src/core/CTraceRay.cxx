import CTraceRay;

import CVector3;
import globals;
import offsets;

#include"win32api.h"

import <cstdint>;

const std::uint8_t* const g_ac_client_exe = reinterpret_cast<const std::uint8_t* const>(GetModuleHandle(__TEXT("ac_client.exe")));

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
* bool a1@<cl>				a1 is passed via register cl
* CPlayer* pPlayer@<eax>	pPlayer is passed via register eax
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

class CPlayer;

CTraceRay CTraceRay::traceLine(
	_In_ const CVector3& vec3PositionFrom,
	_In_ const CVector3& vec3PositionTo,
	_In_opt_ const CPlayer* const pPlayer,
	_In_opt_ const bool bCheckPlayers,
	_In_opt_ const bool bSomeBoolSetToFalse
) noexcept
{
	CTraceRay traceRay = CTraceRay{ };

	__asm {
		push dword ptr[bSomeBoolSetToFalse]
		push dword ptr[bCheckPlayers]
		push dword ptr[pPlayer]


		mov ecx, dword ptr[vec3PositionTo]
		push dword ptr[ecx + 0x8]
		push dword ptr[ecx + 0x4]
		push dword ptr[ecx]

		mov ecx, dword ptr[vec3PositionFrom]
		push dword ptr[ecx + 0x8]
		push dword ptr[ecx + 0x4]
		push dword ptr[ecx]

		lea eax, dword ptr[traceRay]

		call dword ptr[g_pTraceLineFn]
		add esp, 0x24
	}

	return traceRay;
}

bool CTraceRay::isVisible(
	const CVector3& vec3PositionFrom,
	const CVector3& vec3PositionTo
) noexcept
{
	__asm {
		mov eax, dword ptr[vec3PositionTo]
		push dword ptr[eax + 0x8]
		push dword ptr[eax + 0x4]
		push dword ptr[eax]

		mov eax, dword ptr[vec3PositionFrom]
		push dword ptr[eax + 0x8]
		push dword ptr[eax + 0x4]
		push dword ptr[eax]

		xor eax, eax
		xor cl, cl

		call dword ptr[g_pIsVisibleFn]

		add esp, 0x18
	}
}

CROSSHAIR_ID CTraceRay::intersect(
	_In_ const CPlayer& refPlayer,
	_In_ const CVector3& vec3PositionFrom,
	_In_ const CVector3& vec3PositionTo
) noexcept
{
	__asm {
		mov eax, dword ptr[refPlayer]

		mov ecx, dword ptr[vec3PositionTo]
		mov edx, dword ptr[vec3PositionFrom]

		push ecx
		push edx

		xor ebx, ebx

		call dword ptr[g_pIntersectFn]

		add esp, 0x08
	}
}