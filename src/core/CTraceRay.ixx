#include<Windows.h>

export module CTraceRay;

import CVector3;

import <cstdint>;

export class playerent;

export enum class CROSSHAIR_ID : std::uint32_t {
	CROSSHAIR_ID_NO_ENTITY = NULL,
	CROSSHAIR_ID_BODY,
	CROSSHAIR_ID_HEAD
};

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
	//[[nodiscard]] static CROSSHAIR_ID intersect(
	//	/*playerent& refPlayer@<eax>,
	//	CVector3 vec3Delta@<ebx>*/
	//	_In_ const playerent& refPlayer,
	//	_In_ const CVector3& vec3PositionFrom,
	//	_In_ const CVector3& vec3PositionTo
	//) noexcept;
public:
	/*
	* CTraceRay traceResult is passed via register eax
	*/
	typedef void(__cdecl* _traceLine_t)(
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
	inline static CTraceRay::_traceLine_t _pTraceLineFn = nullptr;

	/*
	* bool bSkipCheckingSolids is passed via register cl
	* playerent* pPlayer is passed via register eax
	*/
	typedef bool(__cdecl* _entityIsVisible_t)(
		/*bool bSkipCheckingSolids@<cl>,
		playerent* pPlayer@<eax>*/
		_In_ const float fPositionFromX,
		_In_ const float fPositionFromY,
		_In_ const float fPositionFromZ,
		_In_ const float fPositionToX,
		_In_ const float fPositionToY,
		_In_ const float fPositionToZ
	) noexcept;
	inline static CTraceRay::_entityIsVisible_t _pIsVisibleFn = nullptr;

	/*
	* playerent& refPlayer is passed via register eax
	* CVector3 vec3Delta is passed via register ebx
	*/
	typedef CROSSHAIR_ID(__cdecl* _intersect_t)(
		/*playerent& refPlayer@<eax>,
		CVector3 vec3Delta@<ebx>*/
		_In_ const CVector3 vec3PositionFrom,
		_In_ const CVector3 vec3PositionTo
	) noexcept;
	inline static CTraceRay::_intersect_t _pIntersectFn = nullptr;
public:
	CVector3 vec3EndTrajectory = CVector3{ };
	bool bCollided = false;
};