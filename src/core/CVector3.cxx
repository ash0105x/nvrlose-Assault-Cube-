import CVector3;

import CVector2;

import<sal.h>;
import<cassert>;

#include<math.h>

[[nodiscard]] CVector3::CVector3(
	_In_ const float x,
	_In_ const float y,
	_In_ const float z
) noexcept 
	: x(x), y(y), z(z)
{ }

bool CVector3::isZero( void ) const noexcept {
	return this->x == 0.f && this->y == 0.f && this->z == 0.f;
}

CVector3 CVector3::scale(
	_In_ const float fFactor
) const noexcept
{
	return(
		CVector3{
			this->x * fFactor,
			this->y * fFactor,
			this->z * fFactor
		}
	);
}

float CVector3::length( void ) const noexcept {
	// H = sqrt(K² + K²)
	return (
		sqrtf(
			(this->x * this->x) +
			(this->y * this->y) +
			(this->z * this->z)
		)
	);
}

float CVector3::distance(
	_In_ const CVector3& vec3
) const noexcept
{
	return (vec3 - *this).length();
}

CVector3 CVector3::sphericalToCartesian(
	_In_ const CVector2& refVec2
) noexcept
{
	return(
		CVector3::sphericalToCartesian(
			CVector3{
				1.f,
				refVec2.x,
				refVec2.y
			}
		)
	);
}

CVector3 CVector3::sphericalToCartesian(
	_In_ const CVector3& refVec3
) noexcept
{
	const auto degreesToRadians = [](const float fDegrees) noexcept -> float {
		return fDegrees * static_cast<const float>(3.14159265358979323846 / 180.0);
	};

	const CVector2 vec2Radians = CVector2{
		degreesToRadians(refVec3.y),
		degreesToRadians(refVec3.z)
	};

	const float fCosOfYRadians = cosf(vec2Radians.x);

	// r = x
	// theta = y
	// phi = z

	return(
		CVector3{
			// spherical to Cartesian conversion formula, where x = r * cos(theta) * cos(phi)
			refVec3.x * fCosOfYRadians * cosf(vec2Radians.y),
			// spherical to Cartesian conversion formula, where y = r * cos(theta) * sin(phi)
			refVec3.x * fCosOfYRadians * sinf(vec2Radians.y),
			// spherical to Cartesian conversion formula, where z = -r * sin(theta)
			-refVec3.x * sinf(vec2Radians.x)
		}
	);
}

float& CVector3::operator[](
	_In_ const std::uint8_t u8Index
) noexcept
{
	assert(u8Index <= 2u);

	return reinterpret_cast<float* const>(this)[u8Index];
}
const float& CVector3::operator[](
	_In_ const std::uint8_t u8Index
) const noexcept
{
	assert(u8Index <= 2u);

	return reinterpret_cast<const float* const>(this)[u8Index];
}

CVector3 CVector3::operator+(
	_In_ const CVector3& other
) const noexcept
{
	return(
		CVector3{
			this->x + other.x,
			this->y + other.y,
			this->z + other.z
		}
	);
}

CVector3 CVector3::operator-(
	_In_ const CVector3& other
) const noexcept
{
	return(
		CVector3{
			this->x - other.x,
			this->y - other.y,
			this->z - other.z
		}
	);
}

CVector3 CVector3::operator*(
	_In_ const CVector3& other
) const noexcept
{
	return(
		CVector3{
			this->x * other.x,
			this->y * other.y,
			this->z * other.z
		}
	);
}

CVector3 CVector3::operator/(
	_In_ const CVector3& other
) const noexcept
{
	return(
		CVector3{
			this->x / other.x,
			this->y / other.y,
			this->z / other.z
		}
	);
}

CVector3& CVector3::operator+=(
	_In_ const CVector3& other
) noexcept
{
	*this = *this + other;

	return *this;
}

CVector3& CVector3::operator-=(
	_In_ const CVector3& other
) noexcept
{
	*this = *this - other;

	return *this;
}

CVector3& CVector3::operator*= (
	_In_ const CVector3& other
) noexcept
{
	*this = *this * other;

	return *this;
}

CVector3& CVector3::operator/=(
	_In_ const CVector3& other
) noexcept
{
	*this = *this / other;

	return *this;
}