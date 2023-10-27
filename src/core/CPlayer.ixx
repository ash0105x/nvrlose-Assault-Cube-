export module CPlayer;

import <cstdint>;

import offsets;

export import CVector2;
export import CVector3;

export enum class FORWARD_STATE : std::uint8_t {
	FORWARD_STATE_FORWARD = 0x1u,
	FORWARD_STATE_BACKWARD = 0xFFu
};

export enum class SIDE_STATE : std::uint8_t {
	SIDE_STATE_LEFT = 0x1u,
	SIDE_STATE_RIGHT = 0xFFu
};

export enum class WEAPON : std::uint8_t {
	WEAPON_CARBINE = 2u,
	WEAPON_SHOTGUN = 3u,
	WEAPON_SUBMACHINE_GUN = 4u,
	WEAPON_SNIPER = 5u,
	WEAPON_ASSAULT_RIFLE = 6u,
};

export enum class RVSF_PLAYER_SKIN : std::uint8_t {
	RVSF_PLAYER_SKIN_ALPHA = NULL,
	RVSF_PLAYER_SKIN_TANGO,
	RVSF_PLAYER_SKIN_COBRA,
	RVSF_PLAYER_SKIN_VIPER,
	RVSF_PLAYER_SKIN_SWAT,
	RVSF_PLAYER_SKIN_POLICE
};

export enum class CLA_PLAYER_SKIN : std::uint8_t {
	CLA_PLAYER_SKIN_COMANDANTE = NULL,
	CLA_PLAYER_SKIN_PSYCHO,
	CLA_PLAYER_SKIN_BOMBER,
	CLA_PLAYER_SKIN_RIPPER
};

export enum class STANDING_TYPE : std::uint32_t {
	STANDING_TYPE_STANDING = 0x1000000u,
	STANDING_TYPE_DUCKING = 0x1010001u
};

export enum class AIR_TYPE : std::uint32_t {
	AIR_TYPE_STILL = 0x100u,
	AIR_TYPE_ON_AIR = 0x0u,
	AIR_TYPE_ON_LADDER = 0x10000u,
	AIR_TYPE_JUMP = 0x1010000u
};

export enum class PLAYER_STATE : std::uint8_t {
	PLAYER_STATE_DEAD = 0x1u,
	PLAYER_STATE_GHOST = 0x4u
};

typedef float DUCK_STATE;
export constexpr const DUCK_STATE DUCK_STATE_IN_UN_DUCKING_STATE = .600f;
export constexpr const DUCK_STATE DUCK_STATE_IN_DUCKING_STATE = -::DUCK_STATE_IN_UN_DUCKING_STATE;

constexpr const std::uint8_t MAX_PLAYER_NICKNAME_CHARACTER_COUNT = 16u;

// Created with ReClass.NET 1.2 by KN4CK3R

export class CPlayer final {
public:
	void* vpVTable; // 0x0000
	CVector3 vec3EyePosition; //0x0004
private:
	const char pad_0010[24]; //0x0010
public:
	CVector3 vec3Velocity; //0x0028
	CVector3 vec3FeetPosition; //0x0034
	CVector2 vec2ViewAngles; //0x0040
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
	FORWARD_STATE forwardWalkingState; //0x0080
	SIDE_STATE sidewalkingState; //0x0081
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
	std::uint32_t uHealth; //0x00F8
private:
	const char pad_00FC[8]; //0x00FC
public:
	WEAPON selectedWeaponIndex; //0x0104
private:
	const char pad_0105[15]; //0x0105
public:
	std::uint32_t uSecondaryWeaponChamber; //0x0114
	std::uint32_t uCarbineChamber; //0x0118
	std::uint32_t uShotgunChamber; //0x011C
	std::uint32_t uSubmachineGunChamber; //0x0120
	std::uint32_t uSniperChamber; //0x0124
	std::uint32_t uAssaultRifleChamber; //0x0128
private:
	const char pad_012C[16]; //0x012C
public:
	std::uint32_t uSecondaryWeaponAmmo; //0x013C
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
	std::uint32_t uSecondaryWeaponDelay; //0x0164
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
	std::uint32_t uSecondaryWeaponShotsFiredInCurrentMatch; //0x018C
	std::uint32_t uCarbineShotsFiredInCurrentMatch; //0x0190
	std::uint32_t uShotgunShotsFiredInCurrentMatch; //0x0194
	std::uint32_t uSubmachineGunShotsFiredInCurrentMatch; //0x0198
	std::uint32_t uSniperShotsFiredInCurrentMatch; //0x019C
	std::uint32_t uAssaultRifleShotsFiredInCurrentMatch; //0x01A0
private:
	const char pad_01A4[4]; //0x01A4
public:
	std::uint32_t uGrenadesThrownInCurrentMatch; //0x01A8
private:
	const char pad_01AC[48]; //0x01AC
public:
	CLA_PLAYER_SKIN CLA_PlayerSkinIndex; //0x01DC
private:
	const char pad_01DD[3]; //0x01DD
public:
	RVSF_PLAYER_SKIN RVSF_PlayerSkinIndex; //0x01E0
private:
	const char pad_01E1[3]; //0x01E1
public:
	std::uint8_t u8Index; //0x01E4
private:
	const char pad_01E5[64]; //0x01E5
public:
	char cstrNickname[::MAX_PLAYER_NICKNAME_CHARACTER_COUNT]; //0x0225
}; //Size: 0x235
//static_assert(sizeof(CPlayer) == 0x235);