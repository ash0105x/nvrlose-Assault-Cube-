export module CPlayer;

import <cstdint>;

class Vector3 final {
	float x, y, z;
};

class Vector2 final {
	float x, y;
};

export typedef enum : std::uint32_t {
	MOVE_TYPE_FORWARD = 1u,
	MOVE_TYPE_BACKWARD = 255u,
	MOVE_TYPE_LEFT = 256u,
	MOVE_TYPE_RIGHT = 65280
}MOVE_TYPE;

// Created with ReClass.NET 1.2 by KN4CK3R

// vec3EyePosition = playerent + 0x0004
// vec2Velocity = playerent + 0x0028
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

export class CPlayer final {
private:
	void* N00000053; //0x0000
public:
	Vector3 vec3EyePosition; //0x0004
private:
	char pad_0010[24]; //0x0010
public:
	Vector2 vec2Velocity; //0x0028
private:
	char pad_0030[8]; //0x0030
public:
	Vector3 vec3FeetPosition; //0x0038
	Vector2 vec2ViewAngles; //0x0044
private:
	char pad_004C[20]; //0x004C
public:
	float fCameraHeight; //0x0060
private:
	char pad_0064[4]; //0x0064
	std::uint8_t N00000067; //0x0068
public:
	bool bOnGround; //0x0069
private:
	char pad_006A[6]; //0x006A
public:
	bool bIsNotShooting; //0x0070
private:
	char pad_0071[15]; //0x0071
public:
	MOVE_TYPE i32MoveType; //0x0080 
private:
	char pad_0084[8]; //0x0084
public:
	std::int32_t i32MoveType2; //0x008C
private:
	char pad_0090[108]; //0x0090
public:
	std::int32_t i32Health; //0x00FC
private:
	char pad_0100[80]; //0x0100
public:
	std::int32_t i32Ammo; //0x0150
private:
	std::int32_t asdf; //0x0154
	char pad_0158[12]; //0x0158
public:
	std::int32_t i32FiringSpeedWithSecondaryWeapon; //0x0164
private:
	char pad_0168[16]; //0x0168
public:
	std::int32_t i32FiringSpeedWithPrimaryWeapon; //0x0178
private:
	char pad_017C[12]; //0x017C
public:
	std::uint32_t i32KnifeUseCount; //0x0188
	std::uint32_t i32ShotsFiredWithSecondaryWeapon; //0x018C
private:
	char pad_0190[16]; //0x0190
public:
	std::int32_t i32ShotsFiredWithPrimaryWeapon; //0x01A0
};