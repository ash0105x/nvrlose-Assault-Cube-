export module CTraceRay;

import CVector3;

import <cstdint>;

export enum class CROSSHAIR_ID : std::uint32_t {
	CROSSHAIR_ID_NO_ENTITY = NULL,
	CROSSHAIR_ID_BODY,
	CROSSHAIR_ID_HEAD
};

export class CPlayer;

export class CTraceRay final {
public:
	[[nodiscard]] void traceLine(
		_In_ const CVector3& vec3PositionStart,
		_In_ const CVector3& vec3PositionEnd,
		_In_opt_ const CPlayer* const pTracer,
		_In_opt_ const bool bCheckPlayers,
		_In_opt_ const bool bSkipCheckingSolids
	) noexcept;
public:
	[[nodiscard]] static bool entityIsVisible(
		_In_ const CVector3& vec3PositionFrom,
		_In_ const CVector3& vec3PositionTo
	) noexcept;
	[[nodiscard]] static CROSSHAIR_ID intersect(
		_In_ const CPlayer& refPlayer,
		_In_ const CVector3& vec3PositionFrom,
		_In_ const CVector3& vec3PositionTo
	) noexcept;
public:
	CVector3 vec3EndTrajectory = CVector3{ };
	bool bCollided = false;
};