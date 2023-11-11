export module playerent;

import<cstdint>;

import offsets;
import CTraceRay;
import weapon;

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

export enum class PLAYER_STATE : std::uint8_t {
	PLAYER_STATE_DEAD = 0x1u,
	PLAYER_STATE_GHOST = 0x4u
};

export typedef enum : std::uint8_t {
	LIFE_STATE_EX_ALIVE = 0,
	LIFE_STATE_EX_DEAD = 1u,
	LIFE_STATE_EX_GHOST = 5u
}LIFE_STATE_EX;

export enum class LIFE_STATE : std::int32_t {
	LIFE_STATE_DEAD = -2,
	LIFE_STATE_ALIVE = -4
};

export typedef bool TEAM_ID;
export constexpr const TEAM_ID TEAM_ID_CLA = false;
export constexpr const TEAM_ID TEAM_ID_RVSF = true;

static constexpr const std::uint8_t MAX_PLAYER_NICKNAME_CHARACTER_COUNT = 16u;

// Created with ReClass.NET 1.2 by KN4CK3R

export class playerent final {
public:
	bool isVisibleTo(
		_In_ const CVector3& vec3PlayerPosition
	) const noexcept
	{
		return CTraceRay::entityIsVisible(vec3PlayerPosition, this->vec3EyePosition);
	}
public:
	void* vpVTable; // 0x0000
	CVector3 vec3EyePosition; //0x0004
private:
	const char pad_0010[24]; //0x0010
public:
	CVector3 vec3Velocity; //0x0028
	CVector3 vec3Origin; //0x0034
	CVector2 vec2ViewAngles; //0x0040
private:
	const char pad_0048[12]; //0x0048
public:
	std::uint16_t uFallingSpeed; //0x0054
private:
	const char pad_0056[6]; //0x0056
public:
	float fCameraHeight; //0x005C
	float fMaxCameraHeight; //0x0060
private:
	const char pad_0064[4]; //0x0064
	const std::uint8_t N00000066; //0x0068
public:
	bool bIsOnGround; //0x0069
	bool bIsOnLadder;
public:
	bool bIsFlyingUpwards; //0x006B
	bool bIsDucking; //0x006C
	bool bIsDuckingWhileJumping; //0x006D
	bool bIsFlyingDownwards; //0x006E
private:
	const char pad_006F[1]; //0x006F
public:
	bool bIsStandingStillOrIsNotShooting; //0x0070
	bool bIsScoping; //0x0071
private:
	const char pad_0072[14]; //0x0072
public:
	std::int16_t uWalkingState; //0x0080
	PLAYER_STATE uPlayerState; //0x0082
private:
	const char pad_0083[9]; //0x0083
public:
	bool bIsWalkingLeftSide; //0x008C
	bool bIsWalkingRightSide; //0x008D
	bool bIsWalkingForward; //0x008E
	bool bIsWalkingBackward; //0x008F
private:
	const char pad_0090[105 - 4]; //0x0090
public:
	std::int32_t iHealth; //0x00F8
	std::int32_t iArmor;  //0x00FC
private:
	const char pad_00FC[4]; //0x00FC
public:
	WEAPON_ID uSelectedWeaponID; //0x0104
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
	std::uint8_t uIndex; //0x01E4
private:
	const char pad_01E5[19]; //0x01E5
public:
	std::uint32_t uConfirmedDeaths; //0x01F8
	std::int32_t iFrags; //0x01FC
private:
	const char pad_0200[4]; //0x0200
public:
	std::uint32_t uDeaths; //0x0204
private:
	const char pad_0208[4]; //0x0208
public:
	std::uint32_t uTeamKills; //0x020C
private:
	const char pad_0210[20]; //0x0210
public:
	bool bIsShooting; //0x0224
	char cstrNickname[16]; //0x0225
private:
	const char pad_0235[247]; //0x0235
public:
	TEAM_ID uTeamID; //0x032C
private:
	const char pad_032D[14]; //0x032D
public:
	LIFE_STATE_EX uLifeStateEx;
	LIFE_STATE iLifeState; //0x033C
private:
	const char pad_0340[4]; //0x0340
	const void* const N0000016B; //0x0344
	const char pad_0348[40]; //0x0348
public:
	weapon* pPreviousWeapon; //0x0370
	weapon* pCurrentWeapon2; //0x0374 // The same as playerent::pCurrentWeapon
	weapon* pCurrentWeapon; //0x0378
	weapon* pConfirmedSelectedWeapon; //0x037C
	weapon* pSelectedWeapon; //0x0380
	weapon* pLastShotFiredWeapon; //0x0384
private:
	const char pad_0388[148]; //0x0388
	const void* const N000001A1; //0x041C
	const char pad_0424[4]; //0x0424
	const playerent* const N000001A4; //0x0428
	const char pad_042C[1548]; //0x042C
}; //Size: 0x0A38
//static_assert(sizeof(playerent) == 0xA38);