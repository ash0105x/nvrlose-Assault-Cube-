export module CVector2;

import <sal.h>;
import <compare>;

class CVector2;
export typedef CVector2 vec2;
export typedef CVector2 vec2_t;
export typedef CVector2 Vector2;

export class CVector2 final {
	[[nodiscard]] CVector2(
		_In_ const float x = 0.f,
		_In_ const float y = 0.f
	) noexcept;
public:
	CVector2(
		_In_ const CVector2&
	) noexcept = default;
	CVector2(
		CVector2&&
	) noexcept = default;
public:
	CVector2& operator=(
		_In_ const CVector2&
	) noexcept = default;
	CVector2& operator=(
		CVector2&&
	) noexcept = default;
public:
	constexpr auto operator<=>(
		_In_ const CVector2&
	) const noexcept = default;
public:
	bool isZero(void) const noexcept;
	CVector2& scale(
		_In_ const float fFactor
	) noexcept;
public:
	CVector2 operator+(
		_In_ const CVector2&
	) const noexcept;
	CVector2 operator-(
		_In_ const CVector2&
	) const noexcept;
	CVector2 operator*(
		_In_ const CVector2&
	) const noexcept;
	CVector2 operator/(
		_In_ const CVector2&
	) const noexcept;
public:
	CVector2& operator+=(
		_In_ const CVector2&
	) noexcept;
	CVector2& operator-=(
		_In_ const CVector2&
	) noexcept;
	CVector2& operator*=(
		_In_ const CVector2&
	) noexcept;
	CVector2& operator/=(
		_In_ const CVector2&
	) noexcept;
public:
	float x = 0.f;
	float y = 0.f;
};