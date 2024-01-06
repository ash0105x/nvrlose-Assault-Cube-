export module CVector3;

import CVector2;

import<sal.h>;
import<compare>;

export class CVector3;
export typedef CVector3 vertex3d_t;

export class CVector3 final {
public:
	[[nodiscard]] constexpr CVector3(
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
	constexpr const auto operator<=>(
		_In_ const CVector3&
	) const noexcept = default;
public:
	bool isZero( void ) const noexcept;
	float length( void ) const noexcept;
	float distance(
		_In_ const CVector3& vec3
	) const noexcept;
	CVector3 scale(
		_In_ const float fFactor
	) const noexcept;
public:
	// radius = 1
	static CVector3 sphericalToCartesian(
		_In_ const CVector2& refVec2
	) noexcept;
	static CVector3 sphericalToCartesian(
		_In_ const CVector3& refVec3
	) noexcept;
public:
	float& operator[](
		_In_ const std::uint8_t u8Index
	) noexcept;
	const float& operator[](
		_In_ const std::uint8_t u8Index
	) const noexcept;
public:
	CVector3 operator+(
		_In_ const CVector3&
	) const noexcept;
	CVector3 operator-(
		_In_ const CVector3&
	) const noexcept;
	CVector3 operator*(
		_In_ const CVector3&
	) const noexcept;
	CVector3 operator/(
		_In_ const CVector3&
	) const noexcept;
public:
	CVector3& operator+=(
		_In_ const CVector3&
	) noexcept;
	CVector3& operator-=(
		_In_ const CVector3&
	) noexcept;
	CVector3& operator*= (
		_In_ const CVector3&
	) noexcept;
	CVector3& operator/=(
		_In_ const CVector3&
	) noexcept;
public:
	inline static constexpr const float fDegreesRadiansConversionValue = static_cast<const float>(180.0 / 3.14159265358979323846); // pi;
public:
	union {
		struct {
			float x;

			union {
				struct {
					float y;
					float z;
				};

				CVector2 yz;
			};
		};
		
		CVector2 xy;
	};
};