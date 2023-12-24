import CVector2;

import<sal.h>;
import<cassert>;

#include<math.h>

[[nodiscard]] CVector2::CVector2(
	_In_ const float x,
	_In_ const float y
) noexcept
	: x(x), y(y)
{ }

bool CVector2::isZero( void ) const noexcept {
	return this->x == 0.f && this->y == 0.f;
}

float CVector2::length( void ) const noexcept {
	return (
		sqrtf(
			(this->x * this->x) +
			(this->y * this->y)
		)
	);
}

float CVector2::distance(
	_In_ const CVector2& vec2
) const noexcept
{
	return (vec2 - *this).length();
}

CVector2 CVector2::scale(
	_In_ const float fFactor
) const noexcept
{
	return CVector2{
		this->x * fFactor,
		this->y * fFactor
	};
}

float& CVector2::operator[](
	_In_ const std::uint8_t u8Index
) noexcept
{
	assert(u8Index <= 1u);

	return reinterpret_cast<float* const>(this)[u8Index];
}
const float& CVector2::operator[](
	_In_ const std::uint8_t u8Index
) const noexcept
{
	assert(u8Index <= 1u);

	return reinterpret_cast<const float* const>(this)[u8Index];
}

CVector2 CVector2::operator+(
	_In_ const CVector2& other
) const noexcept
{
	return(
		CVector2{
			this->x + other.x,
			this->y + other.y
		}
	);
}

CVector2 CVector2::operator-(
	_In_ const CVector2& other
) const noexcept
{
	return(
		CVector2{
			this->x - other.x,
			this->y - other.y
		}
	);
}

CVector2 CVector2::operator*(
	_In_ const CVector2& other
) const noexcept
{
	return(
		CVector2{
			this->x * other.x,
			this->y * other.y
		}
	);
}

CVector2 CVector2::operator/(
	_In_ const CVector2& other
) const noexcept
{
	return(
		CVector2{
			this->x / other.x,
			this->y / other.y
		}
	);
}

CVector2& CVector2::operator+=(
	_In_ const CVector2& other
) noexcept
{
	*this = *this + other;

	return *this;
}

CVector2& CVector2::operator-=(
	_In_ const CVector2& other
) noexcept
{
	*this = *this - other;

	return *this;
}

CVector2& CVector2::operator*=(
	_In_ const CVector2& other
) noexcept
{
	*this = *this * other;

	return *this;
}

CVector2& CVector2::operator/=(
	_In_ const CVector2& other
) noexcept
{
	*this = *this / other;

	return *this;
}