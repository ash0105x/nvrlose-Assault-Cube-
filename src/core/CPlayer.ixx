export module CPlayer;

import <cstdint>;

export struct Vector2 final {
	float x = 0.f;
	float y = 0.f;
};

export struct Vector3 final {
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
};

export typedef enum : std::uint8_t {
	FORWARD_STATE_FORWARD = 0x1u,
	FORWARD_STATE_BACKWARD = 0xFFu
}FORWARD_STATE;

export typedef enum : std::uint8_t {
	SIDE_STATE_LEFT = 0x1u,
	SIDE_STATE_RIGHT = 0xFFu
}SIDE_STATE;

export typedef enum : std::uint8_t {
	WEAPON_CARBINE = 2u,
	WEAPON_SHOTGUN = 3u,
	WEAPON_SUBMACHINE_GUN = 4u,
	WEAPON_SNIPER = 5u,
	WEAPON_ASSAULT_RIFLE = 6u,
}WEAPON;

export typedef enum : std::uint8_t {
	RVSF_PLAYER_SKIN_ALPHA = NULL,
	RVSF_PLAYER_SKIN_TANGO,
	RVSF_PLAYER_SKIN_COBRA,
	RVSF_PLAYER_SKIN_VIPER,
	RVSF_PLAYER_SKIN_SWAT,
	RVSF_PLAYER_SKIN_POLICE
}RVSF_PLAYER_SKIN;

export typedef enum : std::uint8_t {
	CLA_PLAYER_SKIN_COMANDANTE = NULL,
	CLA_PLAYER_SKIN_PSYCHO,
	CLA_PLAYER_SKIN_BOMBER,
	CLA_PLAYER_SKIN_RIPPER
}CLA_PLAYER_SKIN;

export typedef enum : std::uint32_t {
	STANDING_TYPE_STANDING = 0x1000000u,
	STANDING_TYPE_DUCKING = 0x1010001u
}STANDING_TYPE;

export typedef enum : std::uint32_t {
	AIR_TYPE_STILL = 0x100u,
	AIR_TYPE_ON_AIR = 0x0u,
	AIR_TYPE_ON_LADDER = 0x10000u,
	AIR_TYPE_JUMP = 0x1010000u
}AIR_TYPE;

export typedef enum : std::uint8_t {
	PLAYER_STATE_DEAD = 0x1u,
	PLAYER_STATE_GHOST = 0x4u
}PLAYER_STATE;

typedef float DUCK_STATE;
export constexpr const DUCK_STATE DUCK_STATE_IN_UN_DUCKING_STATE = .600f;
export constexpr const DUCK_STATE DUCK_STATE_IN_DUCKING_STATE = -::DUCK_STATE_IN_UN_DUCKING_STATE;

// Created with ReClass.NET 1.2 by KN4CK3R

export class CPlayer final {
private:
	const char pad_0000[4]; //0x0000
public:
	Vector3 vec3EyePosition; //0x0004
private:
	const char pad_0010[24]; //0x0010
public:
	Vector3 vec3Velocity; //0x0028
	Vector3 vec3FeetPosition; //0x0034
	Vector2 vec2ViewAngles; //0x0040
private:
	const char pad_0048[20]; //0x0048
public:
	float fCameraHeight; //0x005C
	float fMaxCameraHeight; //0x0060
private:
	const char pad_0064[12]; //0x0064
public:
	bool bIsStandingStillOrIsNotShooting; //0x0070
private:
	const char pad_0071[15]; //0x0071
public:
	FORWARD_STATE forwardState; //0x0080
	SIDE_STATE sideState; //0x0081
	PLAYER_STATE playerState; //0x0082
private:
	const char pad_0083[9]; //0x0083
public:
	bool bIsWalkingLeftSide; //0x008C
	bool bIsWalkingRightSide; //0x008D
	bool bIsWalkingForward; //0x008E
	bool bIsWalkingBackward; //0x008F
private:
	const char pad_0090[104]; //0x0090
public:
	std::uint32_t iHealth; //0x00F8
private:
	const char pad_00FC[8]; //0x00FC
public:
	WEAPON u8SelectedWeaponIndex; //0x0104
private:
	const char pad_0105[15]; //0x0105
public:
	std::uint32_t uSecondaryChamber; //0x0114
	std::uint32_t uCarbineChamber; //0x0118
	std::uint32_t uShotgunChamber; //0x011C
	std::uint32_t uSubmachineGunChamber; //0x0120
	std::uint32_t uSniperChamber; //0x0124
	std::uint32_t uAssaultRifleChamber; //0x0128
private:
	std::uint32_t adfasfasfasfasfas; //0x012C
	std::uint32_t asdfasfasfasd; //0x0130
	std::uint32_t asdfas; //0x0134
	std::uint32_t asdfass; //0x0138
public:
	std::uint32_t uSecondaryAmmo; //0x013C
	std::uint32_t uCarbineAmmo; //0x0140
	std::uint32_t uShotgunAmmo; //0x0144
	std::uint32_t uSubmachineGunAmmo; //0x0148
	std::uint32_t uSniperAmmo; //0x014C
	std::uint32_t uAssaultRifleAmmo; //0x0150
private:
	const char pad_0154[4]; //0x0154
public:
	std::uint32_t uGrenades; //0x0158
private:
	const char pad_015C[4]; //0x015C
public:
	std::uint32_t uKnifeDelay; //0x0160
	std::uint32_t uSecondaryDelay; //0x0164
	std::uint32_t uCarbineDelay; //0x0168
	std::uint32_t uShotgunDelay; //0x016C
	std::uint32_t uSubmachineGunDelay; //0x0170
	std::uint32_t uSniperDelay; //0x0174
	std::uint32_t uAssaultRifleDelay; //0x0178
private:
	const char pad_017C[4]; //0x017C
public:
	std::uint32_t uGrenadeDelay; //0x0180
private:
	const char pad_0184[4]; //0x0184
public:
	std::uint32_t uKnifeUsedInCurrentMatch; //0x0188
	std::uint32_t uShotsFiredInCurrentMatchSecondaryWeapon; //0x018C
	std::uint32_t uShotsFiredInCurrentMatchCarbine; //0x0190
	std::uint32_t uShotsFiredInCurrentMatchShotgun; //0x0194
	std::uint32_t uShotsFiredInCurrentMatchSubmachineGun; //0x0198
	std::uint32_t uShotsFiredInCurrentMatchSniper; //0x019C
	std::uint32_t uShotsFiredInCurrentMatchAssaultRifle; //0x01A0
private:
	const char pad_01A4[4]; //0x01A4
public:
	std::uint32_t uGrenadesThrownInCurrentMatch; //0x01A8
private:
	const char pad_01AC[48]; //0x01AC
public:
	CLA_PLAYER_SKIN CLA_PlayerSkin; //0x01DC
private:
	const char pad_01DD[3]; //0x01DD
public:
	RVSF_PLAYER_SKIN RVSF_PlayerSkin; //0x01E0
private:
	const char pad_01E1[3]; //0x01E1
public:
	std::uint8_t uIndex; //0x01E4
private:
	const char pad_01E5[64]; //0x01E5
public:
	char* cstrNickname; //0x0225
}; //Size: 0x0229

// static_assert(sizeof(CPlayer) == 0x229);