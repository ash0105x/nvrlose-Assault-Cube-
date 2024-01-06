export module playerent;

import<cstdint>;
import<array>;

import CTraceRay;
import CVector2;
import CVector3;

export import weapon;

export typedef enum class _FORWARD_STATE : std::uint8_t {
	FORWARD_STATE_FORWARD = 0x1u,
	FORWARD_STATE_BACKWARD = 0xFFu
}FORWARD_STATE;

export typedef enum class _SIDE_STATE : std::uint8_t {
	SIDE_STATE_LEFT = 0x1u,
	SIDE_STATE_RIGHT = 0xFFu
}SIDE_STATE;

export typedef enum class _WEAPON_DATA : std::uint32_t {
	WEAPON_DATA_SECONDARY,
	WEAPON_DATA_CARBINE,
	WEAPON_DATA_SHOTGUN,
	WEAPON_DATA_SUBMACHINE_GUN,
	WEAPON_DATA_SNIPERRIFLE,
	WEAPON_DATA_ASSAULT_RIFLE
}WEAPON_DATA;

export typedef enum class _WEAPON_DATA_EX : std::uint32_t {
	WEAPON_DATA_EX_KNIFE,
	WEAPON_DATA_EX_SECONDARY,
	WEAPON_DATA_EX_CARBINE,
	WEAPON_DATA_EX_SHOTGUN,
	WEAPON_DATA_EX_SUBMACHINE_GUN,
	WEAPON_DATA_EX_SNIPERRIFLE,
	WEAPON_DATA_EX_ASSAULT_RIFLE
}WEAPON_DATA_EX;

export typedef enum class _RVSF_PLAYER_SKIN : std::uint8_t {
	RVSF_PLAYER_SKIN_ALPHA = 0x0,
	RVSF_PLAYER_SKIN_TANGO,
	RVSF_PLAYER_SKIN_COBRA,
	RVSF_PLAYER_SKIN_VIPER,
	RVSF_PLAYER_SKIN_SWAT,
	RVSF_PLAYER_SKIN_POLICE
}RVSF_PLAYER_SKIN;

export typedef enum class _CLA_PLAYER_SKIN : std::uint8_t {
	CLA_PLAYER_SKIN_COMANDANTE = NULL,
	CLA_PLAYER_SKIN_PSYCHO,
	CLA_PLAYER_SKIN_BOMBER,
	CLA_PLAYER_SKIN_RIPPER
}CLA_PLAYER_SKIN;

export typedef enum class _PLAYER_STATE : std::uint8_t {
	PLAYER_STATE_ALIVE = 0x0,
	PLAYER_STATE_DEAD = 0x1u,
	PLAYER_STATE_GHOST = 0x4u
}PLAYER_STATE;

export typedef enum class _LIFE_STATE_EX : std::uint8_t {
	LIFE_STATE_EX_ALIVE = 0,
	LIFE_STATE_EX_DEAD = 1u,
	LIFE_STATE_EX_GHOST = 5u
}LIFE_STATE_EX;

export typedef enum class _LIFE_STATE : std::int32_t {
	LIFE_STATE_DEAD = -2,
	LIFE_STATE_ALIVE = -4
}LIFE_STATE;

export typedef bool TEAM_ID;
export constexpr const TEAM_ID TEAM_ID_CLA = false;
export constexpr const TEAM_ID TEAM_ID_RVSF = true;

export constexpr const std::uint8_t MAX_PLAYER_NICKNAME_CHARACTER_COUNT = 16u;

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
	std::array<WEAPON_DATA, 6> uArrReservedAmmo; //0x0114
private:
	const char pad_012C[16]; //0x012C
public:
	std::array<WEAPON_DATA, 6> uArrAmmo; //0x013C
private:
	const char pad_0154[4]; //0x0154
public:
	std::uint32_t uGrenades; //0x0158
private:
	const char pad_015C[4]; //0x015C
public:
	std::array<WEAPON_DATA_EX, 7> uArrDelay; //0x0160
private:
	const char pad_017C[4]; //0x017C
public:
	std::uint32_t uGrenadeDelay; //0x0180
private:
	const char pad_0184[4]; //0x0184
public:
	std::array<WEAPON_DATA_EX, 7> uArrFiredCountInCurrentMatch; //0x0188
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
	char cstrNickname[::MAX_PLAYER_NICKNAME_CHARACTER_COUNT]; //0x0225
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
	weapon* const pPreviousWeapon; //0x0370
	weapon* const pCurrentWeapon2; //0x0374 // The same as playerent::pCurrentWeapon but is updated less frequently
	weapon* const pCurrentWeapon; //0x0378
	weapon* const pConfirmedSelectedWeapon; //0x037C
	weapon* const pSelectedWeapon; //0x0380
	weapon* const pLastShotFiredWeapon; //0x0384
//private:
//	const char pad_0388[148]; //0x0388
//	const void* const N000001A1; //0x041C
//	const char pad_0424[4]; //0x0424
//	const playerent* const N000001A4; //0x0428
//	const char pad_042C[1548]; //0x042C
}; //Size: 0x0A38
//static_assert(sizeof(playerent) == 0xA38);