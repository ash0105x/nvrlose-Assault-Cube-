#include<Windows.h>

export module CTraceRay;

import CVector3;
import offsets;

import <cstdint>;

export enum class CROSSHAIR_ID : std::uint32_t {
	CROSSHAIR_ID_NO_ENTITY = NULL,
	CROSSHAIR_ID_BODY,
	CROSSHAIR_ID_HEAD
};

export class playerent;

std::uint8_t* const g_ac_client_exe = reinterpret_cast<std::uint8_t* const>(GetModuleHandle(__TEXT("ac_client.exe")));

export class CTraceRay final {
public:
	void traceLine(
		/*CTraceRay traceResult@<eax>*/
		_In_ const CVector3& vec3PositionStart,
		_In_ const CVector3& vec3PositionEnd,
		_In_opt_ const playerent* const pTracer,
		_In_opt_ const bool bCheckPlayers,
		_In_opt_ const bool bSkipCheckingSolids
	) noexcept;
public:
	[[nodiscard]] static bool entityIsVisible(
		/*bool bSkipCheckingSolids@<cl>,
		playerent* pPlayer@<eax>*/
		_In_ const CVector3& vec3PositionFrom,
		_In_ const CVector3& vec3PositionTo
	) noexcept;
	[[nodiscard]] static CROSSHAIR_ID intersect(
		/*playerent& refPlayer@<eax>,
		CVector3 vec3Delta@<ebx>*/
		_In_ const playerent& refPlayer,
		_In_ const CVector3& vec3PositionFrom,
		_In_ const CVector3& vec3PositionTo
	) noexcept;
private:
	/*
	* CTraceRay traceResult is passed via register eax
	*/
	typedef void(__cdecl* const _traceLine_t)(
		/*CTraceRay traceResult@<eax>*/
		_In_ const float fPositionStartX,
		_In_ const float fPositionStartY,
		_In_ const float fPositionStartZ,
		_In_ const float fPositionEndX,
		_In_ const float fPositionEndY,
		_In_ const float fPositionEndZ,
		_In_opt_ const int pPlayer,
		_In_opt_ const bool bCheckPlayers,
		_In_opt_ const bool bSomeBoolSetToFalse
	) noexcept;
	inline static const CTraceRay::_traceLine_t _pTraceLineFn = reinterpret_cast<const CTraceRay::_traceLine_t>(::g_ac_client_exe + offsets::ac_client_exe::function::TRACE_LINE);

	/*
	* bool bSkipCheckingSolids is passed via register cl
	* playerent* pPlayer is passed via register eax
	*/
	typedef bool(__cdecl* const _entityIsVisible_t)(
		/*bool bSkipCheckingSolids@<cl>,
		playerent* pPlayer@<eax>*/
		_In_ const float fPositionFromX,
		_In_ const float fPositionFromY,
		_In_ const float fPositionFromZ,
		_In_ const float fPositionToX,
		_In_ const float fPositionToY,
		_In_ const float fPositionToZ
	) noexcept;
	inline static const CTraceRay::_entityIsVisible_t _pIsVisibleFn = reinterpret_cast<const CTraceRay::_entityIsVisible_t>(::g_ac_client_exe + offsets::ac_client_exe::function::IS_VISIBLE);

	/*
	* playerent& refPlayer is passed via register eax
	* CVector3 vec3Delta is passed via register ebx
	*/
	typedef CROSSHAIR_ID(__cdecl* const _intersect_t)(
		/*playerent& refPlayer@<eax>,
		CVector3 vec3Delta@<ebx>*/
		_In_ const CVector3 vec3PositionFrom,
		_In_ const CVector3 vec3PositionTo
	) noexcept;
	inline static const CTraceRay::_intersect_t _pIntersectFn = reinterpret_cast<const CTraceRay::_intersect_t>(::g_ac_client_exe + offsets::ac_client_exe::function::INTERSECT);
	inline static CVector3* const vec3pCoordinates = reinterpret_cast<CVector3* const>(::g_ac_client_exe + 0x10A400u);
public:
	CVector3 vec3EndTrajectory = CVector3{ };
	bool bCollided = false;
};