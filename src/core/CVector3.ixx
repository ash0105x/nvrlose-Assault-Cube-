export module CVector3;

import CVector2;

import <sal.h>;
import <compare>;

class CVector3;
export typedef CVector3 vec;
export typedef CVector3 vec_t;
export typedef CVector3 vec3_t;
export typedef CVector3 Vector3;

export class CVector3 final {
public:
	[[nodiscard]] CVector3(
		_In_ const float x = 0.f,
		_In_ const float y = 0.f,
		_In_ const float z = 0.f
	) noexcept;
	CVector3(
		_In_ const CVector3&
	) noexcept = default;
	CVector3(
		CVector3&&
	) noexcept = default;
public:
	CVector3& operator=(
		_In_ const CVector3&
	) noexcept = default;
	CVector3& operator=(
		CVector3&&
	) noexcept = default;
public:
	constexpr auto operator<=>(
		_In_ const CVector3&
	) const noexcept = default;
public:
	bool isZero(void) const noexcept;
	CVector3& scale(
		_In_ const float fFactor
	) noexcept;
	float length(void) const noexcept;
public:
	CVector3 operator+(
		const CVector3&
	) const noexcept;
	CVector3 operator-(
		const CVector3&
	) const noexcept;
	CVector3 operator*(
		const CVector3&
	) const noexcept;
	CVector3 operator/(
		const CVector3&
	) const noexcept;
public:
	CVector3& operator+=(
		const CVector3&
	) noexcept;
	CVector3& operator-=(
		const CVector3&
	) noexcept;
	CVector3& operator*= (
		const CVector3&
	) noexcept;
	CVector3& operator/=(
		const CVector3&
	) noexcept;
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
};