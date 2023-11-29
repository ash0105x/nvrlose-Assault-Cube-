export module weapon;

import<cstdint>;

export class playerent;

export import gun;

export typedef enum class _WEAPON_ID : std::uint8_t {
	WEAPON_ID_CARBINE = 2u,
	WEAPON_ID_SHOTGUN = 3u,
	WEAPON_ID_SUBMACHINE_GUN = 4u,
	WEAPON_ID_SNIPER = 5u,
	WEAPON_ID_ASSAULT_RIFLE = 6u,
}WEAPON_ID;

// Created with ReClass.NET 1.2 by KN4CK3R

export class weapon final {
public:
	gun* const vpVTable = nullptr; //0x0000
	WEAPON_ID uUniqueID = static_cast<const WEAPON_ID>(NULL); //0x0004
private:
	const char pad_0005[3u] = { }; //0x0005
public:
	playerent* const pOwner = nullptr; //0x0008
	char* const cstrName = nullptr; //0x000C
	std::uint32_t* const upReservedAmmo = nullptr; //0x0010
	std::uint32_t* const upAmmo = nullptr; //0x0014
	std::uint32_t* const upFirerate = nullptr; //0x0018
	std::uint32_t uShotsFired = NULL; //0x001C
	std::uint32_t uReloadTime = NULL; //0x0020
}; //Size: 0x0024
static_assert(sizeof(weapon) == 0x24u);