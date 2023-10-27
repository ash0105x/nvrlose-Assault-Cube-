import CVector2;

import <sal.h>;

[[nodiscard]] CVector2::CVector2(
	_In_ const float x,
	_In_ const float y
) noexcept
	: x(x), y(y)
{ }

bool CVector2::isZero(void) const noexcept {
	return this->x == 0.f && this->y == 0.f;
}

CVector2& CVector2::scale(
	_In_ const float fFactor
) noexcept
{
	this->x *= fFactor;
	this->y *= fFactor;

	return *this;
}

CVector2 CVector2::operator+(
	_In_ const CVector2& other
) const noexcept
{
	return(
		this == &other ?
		*this :
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
		this == &other ?
		*this :
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
		this == &other ?
		*this :
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
		this == &other ?
		*this :
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