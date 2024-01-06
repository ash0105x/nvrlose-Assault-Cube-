export module weapon;

import<cstdint>;

export class playerent;
export class gun;
export class initialWeaponData;

export typedef enum class _WEAPON_ID : std::uint8_t {
	WEAPON_ID_KNIFE,
	WEAPON_ID_PISTOL,
	WEAPON_ID_CARBINE,
	WEAPON_ID_SHOTGUN,
	WEAPON_ID_SUBMACHINE_GUN,
	WEAPON_ID_SNIPER,
	WEAPON_ID_ASSAULT_RIFLE,
	WEAPON_ID_GRENADE = 8u,
	WEAPON_ID_AKIMBO
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
	initialWeaponData* const pInitialWeaponData = nullptr; //0x000C
	std::uint32_t* const upReservedAmmo = nullptr; //0x0010
	std::uint32_t* const upAmmo = nullptr; //0x0014
	std::uint32_t* const upFirerate = nullptr; //0x0018
	std::uint32_t uShotsFired = NULL; //0x001C
	std::uint32_t uReloadTime = NULL; //0x0020
}; //Size: 0x0024
static_assert(sizeof(weapon) == 0x24u);

// Created with ReClass.NET 1.2 by KN4CK3R

export struct initialWeaponData final {
	const char cstrName[8]; //0x0000
private:
	char pad_0008[258]; //0x0008
public:
	std::uint16_t uFirerate; //0x010A
private:
	char pad_010C[12]; //0x010C
public:
	std::uint8_t uAmmo; //0x0118
}; //Size: 0x0119
static_assert(sizeof(initialWeaponData) == 0x11A);