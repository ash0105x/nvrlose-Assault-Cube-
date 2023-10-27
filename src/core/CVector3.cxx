import CVector3;

import <sal.h>;

#include<cmath>

[[nodiscard]] CVector3::CVector3(
	_In_ const float x,
	_In_ const float y,
	_In_ const float z
) noexcept 
	: x(x), y(y), z(z)
{ }

bool CVector3::isZero(void) const noexcept {
	return this->x == 0.f && this->y == 0.f && this->z == 0.f;
}

CVector3& CVector3::scale(
	_In_ const float fFactor
) noexcept
{
	this->x *= fFactor;
	this->y *= fFactor;
	this->z *= fFactor;

	return *this;
}

float CVector3::length(void) const noexcept {
	// H = sqrt(K² + K²)
	return (
		std::sqrt(
			(this->x * this->x) +
			(this->y * this->y) +
			(this->z * this->z)
		)
	);
}

CVector3 CVector3::operator+(
	const CVector3& other
) const noexcept
{
	return(
		this == &other ?
		*this :
		CVector3{
			this->x + other.x,
			this->y + other.y,
			this->z + other.z,
		}
	);
}

CVector3 CVector3::operator-(
	const CVector3& other
) const noexcept
{
	return(
		this == &other ?
		*this :
		CVector3{
			this->x - other.x,
			this->y - other.y,
			this->z - other.z,
		}
	);
}

CVector3 CVector3::operator*(
	const CVector3& other
) const noexcept
{
	return(
		this == &other ?
		*this :
		CVector3{
			this->x * other.x,
			this->y * other.y,
			this->z * other.z,
		}
	);
}

CVector3 CVector3::operator/(
	const CVector3& other
) const noexcept
{
	return(
		this == &other ?
		*this :
		CVector3{
			this->x / other.x,
			this->y / other.y,
			this->z / other.z,
		}
	);
}

CVector3& CVector3::operator+=(
	const CVector3& other
) noexcept
{
	*this = *this + other;

	return *this;
}

CVector3& CVector3::operator-=(
	const CVector3& other
) noexcept
{
	*this = *this - other;

	return *this;
}

CVector3& CVector3::operator*= (
	const CVector3& other
) noexcept
{
	*this = *this * other;

	return *this;
}

CVector3& CVector3::operator/=(
	const CVector3& other
) noexcept
{
	*this = *this / other;

	return *this;
}