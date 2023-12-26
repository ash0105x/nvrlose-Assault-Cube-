export module CVector2;

import<sal.h>;
import<compare>;

export class CVector2 final {
public:
	[[nodiscard]] constexpr CVector2(
		_In_ const float x = 0.f,
		_In_ const float y = 0.f
	) noexcept;
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
	constexpr const auto operator<=>(
		_In_ const CVector2&
	) const noexcept = default;
public:
	bool isZero( void ) const noexcept;
	float length( void ) const noexcept;
	float distance(
		_In_ const CVector2& vec2
	) const noexcept;
	CVector2 scale(
		_In_ const float fFactor
	) const noexcept;
public:
	float& operator[](
		_In_ const std::uint8_t u8Index
	) noexcept;
	const float& operator[](
		_In_ const std::uint8_t u8Index
	) const noexcept;
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