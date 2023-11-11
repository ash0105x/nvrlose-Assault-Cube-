import CTraceRay;

import CVector3;
import playerent;

import<cstdint>;

void CTraceRay::traceLine(
	/*CTraceRay traceResult@<eax>*/ // Result
	_In_ const CVector3& vec3PositionStart, // Starting position of the ray
	_In_ const CVector3& vec3PositionEnd, // Ending position of the ray
	_In_opt_ const playerent* const pTracer, // The player who is tracing the line
	_In_opt_ const bool bCheckPlayers, // Whether to check for players along the line
	_In_opt_ const bool bSkipCheckingSolids // Whether to skip checking for solids along the line
) noexcept
{
	__asm {
		// Pushing parameters onto the stack from right to left as per __cdecl calling convention
		push dword ptr[bSkipCheckingSolids]
		push dword ptr[bCheckPlayers]
		push dword ptr[pTracer]

		// Accessing vec3PositionEnd components and pushing them onto the stack
		mov eax, dword ptr[vec3PositionEnd]
		push dword ptr[eax + 0x8u] // Pushing vec3PositionEnd.z onto the stack
		push dword ptr[eax + 0x4u] // Pushing vec3PositionEnd.y onto the stack
		push dword ptr[eax] // Pushing vec3PositionEnd.x onto the stack

		// Accessing vec3PositionStart components and pushing them onto the stack
		mov eax, dword ptr[vec3PositionStart]
		push dword ptr[eax + 0x8u] // Pushing vec3PositionStart.z onto the stack
		push dword ptr[eax + 0x4u] // Pushing vec3PositionStart.y onto the stack
		push dword ptr[eax] // Pushing vec3PositionStart.x onto the stack

		mov eax, dword ptr[this] // The CTraceRay instance (traceResult) is passed via eax

		call dword ptr[CTraceRay::_pTraceLineFn] // Calling AssaultCubes's TraceLine function

		xor eax, eax // Clearing eax register as this is a void function

		add esp, 0x24u // Cleaning up the stack by adding 0x24 (36 in decimal) to esp register
	}
}

[[nodiscard]] bool CTraceRay::entityIsVisible(
	/*bool bSkipCheckingSolids@<cl>,
	playerent* pPlayer@<eax>*/
	_In_ const CVector3& vec3PositionFrom,
	_In_ const CVector3& vec3PositionTo
) noexcept
{
	__asm {
		// Pushing parameters onto the stack from right to left as per __cdecl calling convention
		// Accessing vec3PositionEnd components and pushing them onto the stack
		mov eax, dword ptr [vec3PositionTo]
		push dword ptr[eax + 0x8u] // Pushing vec3PositionTo.z onto the stack
		push dword ptr[eax + 0x4u] // Pushing vec3PositionTo.y onto the stack
		push dword ptr[eax] // Pushing vec3PositionTo.x onto the stack

		// Accessing vec3PositionStart components and pushing them onto the stack
		mov eax, dword ptr [vec3PositionFrom]
		push dword ptr[eax + 0x8u] // Pushing vec3PositionFrom.z onto the stack
		push dword ptr[eax + 0x4u] // Pushing vec3PositionFrom.y onto the stack
		push dword ptr[eax] // Pushing vec3PositionFrom.x onto the stack

		xor eax, eax // Setting pPlayer to nullptr
		xor cl, cl // Setting bSkipCheckingSolids to false

		call dword ptr[CTraceRay::_pIsVisibleFn] // Calling AssaultCubes's entityIsVisible function

		add esp, 0x18u // Cleaning up the stack by adding 0x18 (24 in decimal) to esp register
	}
}

[[nodiscard]] CROSSHAIR_ID CTraceRay::intersect(
	/*playerent& refPlayer@<eax>,
	CVector3 vec3Delta@<ebx>*/
	_In_ const playerent& refPlayer,
	_In_ const CVector3& vec3PositionFrom,
	_In_ const CVector3& vec3PositionTo
) noexcept
{
	const CVector3 vec3PreviousCoordinates = *CTraceRay::vec3pCoordinates;

	*CTraceRay::vec3pCoordinates = refPlayer.vec3EyePosition;

	__asm {
		mov eax, dword ptr[refPlayer]

		mov ecx, dword ptr[vec3PositionTo]
		mov edx, dword ptr[vec3PositionFrom]

		push ecx
		push edx

		xor ebx, ebx

		call dword ptr[CTraceRay::_pIntersectFn]

		add esp, 0x08u
	}

	*CTraceRay::vec3pCoordinates = vec3PreviousCoordinates;
}