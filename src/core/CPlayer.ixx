export module CPlayer;

import <cstdint>;

export struct Vector3 final {
	float x, y, z;
};

export struct Vector2 final {
	float x, y;
};

export typedef enum : std::uint32_t {
	MOVE_TYPE_FORWARD = 1u,
	MOVE_TYPE_BACKWARD = 255u,
	MOVE_TYPE_LEFT = 256u,
	MOVE_TYPE_RIGHT = 65280
}MOVE_TYPE;

// vec3EyePosition = playerent + 0x0004
// vec3Velocity = playerent + 0x0028
// vec3FeetPosition = playerent + 0x0034
// vec2ViewAngles = playerent + 0x0040
// bOnGround = playerent + 0x69
// fCameraHeight = playerent + 0x005C
// bIsNotShooting = playerent + 0x70
// i32MoveType = playerent + 0x80 -> 1 = forward, 255 = backward, 256 = left, 65280 = right
// i32Health = playerent + 0x00F8
// i32Ammo = playerent + 0x0150
// i32FiringSpeed = playerent + 0x0178
// i32ShotsFired = playerent + 0x01A0
// cstrName = playerent + 0x225

// Created with ReClass.NET 1.2 by KN4CK3R

export class CPlayer final {
public:
	void* vTable = nullptr; //0x0000
	Vector3 vec3EyePosition = Vector3{ 0.f, 0.f, 0.f }; //0x0004
private:
	const char pad_0010[24] = { }; //0x0010
public:
	Vector3 vec3Velocity = Vector3{ 0.f, 0.f, 0.f }; //0x0028
	Vector3 vec3FeetPosition = Vector3{ 0.f, 0.f, 0.f }; //0x0034
	Vector2 vec2ViewAngles = Vector2{ 0.f, 0.f }; //0x0040
private:
	const char pad_0048[20] = { }; //0x0048
public:
	float fCameraHeight = 0.f; //0x005C
private:
	const char pad_0060[8] = { }; //0x0060
	const bool N00000066 = false; //0x0068
public:
	bool bIsOnGround = false; //0x0069
private:
	const char pad_006A[6] = { }; //0x006A
public:
	bool bIsNotShooting = false; //0x0070
private:
	const char pad_0071[15] = { }; //0x0071
public:
	MOVE_TYPE iMoveType = static_cast<const MOVE_TYPE>(NULL); //0x0080
private:
	const char pad_0084[116] = { }; //0x0084
public:
	std::int32_t iHealth = NULL; //0x00F8
private:
	const char pad_00FC[84] = { }; //0x00FC
public:
	std::int32_t iAmmo = NULL; //0x0150
private:
	const char pad_0154[36] = { }; //0x0154
public:
	std::int32_t iFiringSpeed = NULL; //0x0178
private:
	const char pad_017C[36] = { }; //0x017C
public:
	std::int32_t iShotsFiredInCurrentMatch = NULL; //0x01A0
}; //Size: 0x01A4

static_assert(sizeof(CPlayer) == 0x1A4);